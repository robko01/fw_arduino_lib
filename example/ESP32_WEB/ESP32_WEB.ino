/*
    MIT License
    
    Copyright (c) [2019] [Orlin Dimitrov]
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/*

+------+----------+-----------+
| Axis |   Sign   | Direction |
+------+----------+-----------+
|    0 | Positive | CCW       |
|    0 | Negative | CW        |
|    1 | Positive | Forward   |
|    1 | Negative | Backward  |
|    2 | Positive | Backward  |
|    2 | Negative | Forward   |
|    3 | Positive | Down      |
|    3 | Negative | Up        |
|    4 | Positive | CW        |
|    4 | Negative | CCW       |
|    5 | Positive | Open      |
|    5 | Negative | Close     |
+------+----------+-----------+

*/

#pragma region Headers

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#include <NTPClient.h>

#include "ApplicationConfiguration.h"

#include "JointPosition.h"

#include "JointPositionUnion.h"

#include "SUPER.h"

#include "BusConfig.h"

#include "Robko01.h"

#include "DebugPort.h"

#include "OperationsCodes.h"

#include "DefaultCredentials.h"

#include "GeneralHelper.h"

/* Device configuration. */
#include "NetworkConfiguration.h"

#include "DeviceConfiguration.h"

#include "AccessPointConfiguration.h"

#include "FxTimer.h"

/* Base WEB server. */
#include "WEBServer.h"

#ifdef ENABLE_RESCUE_BTN
#include "RescueButton.h"
#endif // ENABLE_RESCUE_BTN

#pragma endregion

#pragma region Classes

class AppWEBServer : public WEBServer {


public:

	/**
	 * @brief Construct a new App W E B Server object
	 * 
	 * @param port , uint16_t WEB server port.
	 */
	AppWEBServer(uint16_t port) : WEBServer(port) {}

	/**
	 * @brief Initialize the application.
	 * 
	 * @param fs File System pointer.
	 */
	void init(FS* fs)
	{
		this->begin(fs);
		// this->setAppRouts();
	}
};

#pragma endregion

#pragma region Prototypes

/**
 * @brief Callback handler function.
 * 
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t * payload);

#pragma endregion

#pragma region Variables

/**
 * @brief Motors state bits.
 * 
 */
uint8_t MotorState_g;

/**
 * @brief Store position flag.
 * 
 */
bool StorePosition_g;

/**
 * @brief Safety stop flag.
 * 
 */
int SafetyStopFlag_g;

/**
 * @brief Move joint in absolute mode.
 * 
 */
JointPositionUnion MoveAbsolute_g;

/**
 * @brief Move joint in relative mode.
 * 
 */
JointPositionUnion MoveRelative_g;

/**
 * @brief Move joint in speed mode.
 * 
 */
JointPositionUnion MoveSpeed_g;

/**
 * @brief Curent joint positions.
 * 
 */
JointPositionUnion CurrentPositions_g;

#ifdef DEAFULT_CREDENTIALS_H_

/**
 * @brief SSID
 * 
 */
const char* SSID_g = DEFAULT_SSID;

/**
 * @brief Password
 * 
 */
const char* PASS_g = DEFAULT_PASS;

#else

/**
 * @brief SSID
 * 
 */
const char* SSID_g = "yourssid";

/**
 * @brief Password
 * 
 */
const char* PASS_g = "yourpasswd";

#endif

WiFiServer TCPServer_g(SERVICE_PORT);

/** @brief NTP UDP socket. */
WiFiUDP NTP_UDP_g;

/** @brief NTP client. */
NTPClient NTPClient_g(NTP_UDP_g);

/** @brief WiFi connection timer. */
FxTimer WiFiConnTimer_g = FxTimer();

/**
 * @brief Application WEB server.
 * 
 * @return AppWEBServer 
 */
AppWEBServer AppWEBServer_g(WEB_SERVER_PORT);

#pragma endregion

/**
 * @brief Setup the peripheral hardware and variables.
 * 
 */
void setup()
{
	// Setup debug port module.
	setup_debug_port(DBG_OUTPUT_PORT_BAUDRATE);

#ifdef ENABLE_RESCUE_BTN
	config_rescue_procedure();
#endif // ENABLE_RESCUE_BTN

	// Show flash state.
	show_device_properties();

	// Start the file system.
	configure_file_system();

	// Try to load configuration from file system. Load defaults if any error.
	if (!load_network_configuration(&SPIFFS, CONFIG_NET))
	{
		save_network_configuration(&SPIFFS, CONFIG_NET);
	}

	// Load HTTP authorization. Load defaults if any error.
	if (!load_device_config(&SPIFFS, CONFIG_DEVICE))
	{
		save_device_config(&SPIFFS, CONFIG_DEVICE);
	}

	// If no SSID and Password wer set the go to AP mode.
	if (NetworkConfiguration.SSID != "" && NetworkConfiguration.Password != "")
	{
		configure_to_sta();
		NTPClient_g.update();

#ifdef ENABLE_STATUS_LED
		StatusLed.setAnumation(AnimationType::Green);
#endif // ENABLE_STATUS_LED
	}
	else
	{
		configure_to_ap();
#ifdef ENABLE_STATUS_LED
		StatusLed.setAnumation(AnimationType::Blue);
#endif // ENABLE_STATUS_LED
	}

	configure_web_server();

#ifdef ENABLE_ARDUINO_OTA
	configure_arduino_ota();
#endif // ENABLE_ARDUINO_OTA

	if ((WiFi.getMode() == WIFI_STA) && WiFi.isConnected())
	{
		SafetyStopFlag_g = LOW;
		MotorState_g = 0;
		StorePosition_g = true;

		// Setup the robot bus driver.
		BusConfig_t config = {
			PIN_AO0,
			PIN_AO1,
			PIN_AO2,
			PIN_IOR,
			PIN_IOW,
			PIN_DI0,
			PIN_DI1,
			PIN_DI2,
			PIN_DI3,
			PIN_DO0,
			PIN_DO1,
			PIN_DO2,
			PIN_DO3,
		};

		// Initialize the robot controller.
		Robko01.init(&config);

		// Initialize the SUPER protocol parser.
		SUPER.setCbRequest(cbRequestHandler);
	}

	// STOP
}

/**
 * @brief Main loop of the program.
 * 
 */
void loop()
{
#ifdef ENABLE_RESQUE_BTN
	update_rescue_procedure();
#endif // ENABLE_RESQUE_BTN

	// If everything is OK with the transport layer.
	if ((WiFi.getMode() == WIFI_STA) && WiFi.isConnected())
	{
		// Update date and time.
		NTPClient_g.update();
	}

	udate_super();

	if (SafetyStopFlag_g == LOW)
	{
        Robko01.update();
        MotorState_g = Robko01.get_motor_state();
	}

	if (MotorState_g == 0 && StorePosition_g)
	{
		StorePosition_g = false;

		// TODO: Save position.
	}
	else
	{
		StorePosition_g = true;
	}
}

#pragma region Functions

#pragma region File System

/**
 * @brief Configure the file system.
 * 
 */
void configure_file_system() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#ifdef ARDUINO_ARCH_ESP32
// ARDUINO_ARCH_ESP32
	if (!SPIFFS.begin(true))
#elif defined(ARDUINO_ARCH_ESP8266)
// ARDUINO_ARCH_ESP8266
	if (!SPIFFS.begin())
#endif
	{
		DEBUGLOG("Can not load file system.\r\n");

		for (;;) {
#ifdef ARDUINO_ARCH_ESP32
// ARDUINO_ARCH_ESP32

#elif defined(ARDUINO_ARCH_ESP8266)
// ARDUINO_ARCH_ESP8266
			ESP.wdtFeed();
#endif
		}
	}

#ifdef DEBUGLOG
#ifdef ARDUINO_ARCH_ESP32
// ARDUINO_ARCH_ESP32
	File root = SPIFFS.open("/");
	File file = root.openNextFile();
	while(file)
	{
		DEBUGLOG("File: %s, size: %s\r\n", file.name(), formatBytes(file.size()).c_str()); 
		file = root.openNextFile();
	}

#elif defined(ARDUINO_ARCH_ESP8266)
// ARDUINO_ARCH_ESP8266
	// List files
	Dir dir = SPIFFS.openDir("/");
	while (dir.next())
	{
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		DEBUGLOG("File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}

#endif

	DEBUGLOG("\r\n");
#endif // DEBUGLOG
}

#pragma endregion

#pragma region AP mode

#ifdef ARDUINO_ARCH_ESP32
// ARDUINO_ARCH_ESP32

/**
 * @brief Configure to AP mode for ESP32.
 * 
 */
void configure_to_ap() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Set the host name.
	// WiFi.hostname(NetworkConfiguration.Hostname.c_str());

	// Set the mode.
	WiFi.mode(WIFI_AP);

	uint32_t chipId = 0;
	for(int i=0; i<17; i=i+8)
	{
		chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}

	// NOTE: See description, this can be used as unique identification of the device.
	AccessPointConfiguration.SSID = String(DEVICE_BRAND) + String(" - ") + String(chipId);

	// Create AP name.
	DEBUGLOG("AP Name: %s\r\n", AccessPointConfiguration.SSID.c_str());

	if (AccessPointConfiguration.Enable) {
		WiFi.softAP(AccessPointConfiguration.SSID.c_str(), AccessPointConfiguration.Password.c_str());
		DEBUGLOG("AP Pass enabled: %s\r\n", AccessPointConfiguration.Password.c_str());
	}
	else {
		WiFi.softAP(AccessPointConfiguration.SSID.c_str());
		DEBUGLOG("AP Pass disabled.\r\n");
	}
}

#elif defined(ARDUINO_ARCH_ESP8266)
// ARDUINO_ARCH_ESP8266

/**
 * @brief Configure to AP mode for ESP8266.
 * 
 */
void configure_to_ap() {
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	// Register event handlers.
	// Callback functions will be called as long as these handler objects exist.
	// Call "handler_ap_mode_station_connected" each time a station connects
	WiFi.onSoftAPModeStationConnected(&handler_ap_mode_station_connected);

	// Call "handler_ap_mode_station_disconnected" each time a station disconnects
	WiFi.onSoftAPModeStationDisconnected(&handler_ap_mode_station_disconnected);

	// Set the host name.
	WiFi.hostname(NetworkConfiguration.Hostname.c_str());

	// Set the mode.
	WiFi.mode(WIFI_AP);

	// NOTE: See description, this can be used as unique identification of the device.
	AccessPointConfiguration.SSID = String(DEVICE_BRAND) + String(" - ") + (String)ESP.getChipId();

	// Create AP name.
	DEBUGLOG("AP Name: %s\r\n", AccessPointConfiguration.SSID.c_str());

	if (AccessPointConfiguration.Enable)
	{
		WiFi.softAP(AccessPointConfiguration.SSID.c_str(), AccessPointConfiguration.Password.c_str());
		DEBUGLOG("AP Pass enabled: %s\r\n", AccessPointConfiguration.Password.c_str());
	}
	else
	{
		WiFi.softAP(AccessPointConfiguration.SSID.c_str());
		DEBUGLOG("AP Pass disabled.\r\n");
	}
}

/** @brief Handler that execute when client is connected.
 *  @param const WiFiEventSoftAPModeStationConnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_connected(const WiFiEventSoftAPModeStationConnected& evt) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Station connected: %s\r\n", mac2str(evt.mac).c_str());
}

/** @brief Handler that execute when client is disconnected.
 *  @param const WiFiEventSoftAPModeStationDisconnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_disconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Station disconnected: %s\r\n", mac2str(evt.mac).c_str());
}

#endif

#pragma endregion

#pragma region STA mode

/**
 * @brief Configure WiFi module to station.
 * 
 */
void configure_to_sta() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Set the host name.
#ifdef ARDUINO_ARCH_ESP32
// ARDUINO_ARCH_ESP32

#elif defined(ARDUINO_ARCH_ESP8266)
// ARDUINO_ARCH_ESP8266
	// Set the host name.
	WiFi.hostname(NetworkConfiguration.Hostname.c_str());
#endif

	// Disconnect required here,
	// improves reconnect reliability.
	WiFi.disconnect();

	// Encourage clean recovery after disconnect species5618, 08-March-2018.
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_STA);

	DEBUGLOG("Connecting: %s\r\n", NetworkConfiguration.SSID.c_str());

	WiFi.begin(NetworkConfiguration.SSID.c_str(), NetworkConfiguration.Password.c_str());

	// Check the DHCP.
	if (!NetworkConfiguration.DHCP)
	{
		DEBUGLOG("DHCP: Disabled\r\n");
		WiFi.config(NetworkConfiguration.IP, NetworkConfiguration.Gateway, NetworkConfiguration.NetMask, NetworkConfiguration.DNS);
	}

	WiFiConnTimer_g.setExpirationTime(TIMEOUT_TO_CONNECT * 1000UL);
	while (WiFi.status() != WL_CONNECTED)
	{ // Wait for the Wi-Fi to connect
		//DEBUGLOG("Stat: %d\r\n", WiFi.status());
		WiFiConnTimer_g.update();
		if (WiFiConnTimer_g.expired())
		{
			WiFiConnTimer_g.clear();
			configure_to_ap();
			return;
		}
		delay(500);
#ifdef ARDUINO_ARCH_ESP32
// ARDUINO_ARCH_ESP32

#elif defined(ARDUINO_ARCH_ESP8266)
// ARDUINO_ARCH_ESP8266
		ESP.wdtFeed();
#endif
	}

	DEBUGLOG("Connected:  %s\r\n", NetworkConfiguration.SSID.c_str());
	DEBUGLOG("IP Address: %s\r\n", WiFi.localIP().toString().c_str());
	DEBUGLOG("Gateway:    %s\r\n", WiFi.gatewayIP().toString().c_str());
	DEBUGLOG("DNS:        %s\r\n", WiFi.dnsIP().toString().c_str());
	DEBUGLOG("\r\n");
}

#pragma endregion

#pragma region Local WEB Server

/**
 * @brief Configure the WEB server.
 * 
 */
void configure_web_server() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	AppWEBServer_g.init(&SPIFFS);
	AppWEBServer_g.setCbReboot(reboot);
}

/**
 * @brief Reboot callback.
 * 
 */
void reboot() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	ESP.restart();
}

#pragma endregion

#pragma region SUPER Robot

/**
 * @brief Update communication.
 * 
 */
void udate_super() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	static uint8_t StateL = 0;
	
	static WiFiClient ClientL;
	
	if (StateL == 0)
	{
		// Start the server.
		TCPServer_g.begin();
		StateL = 1;
	}
	if (StateL == 1)
	{
		// Listen for incoming clients.
		ClientL = TCPServer_g.available();

		if (ClientL)
		{
			SUPER.init(ClientL);
			StateL = 2;
			DEBUGLOG("Connected...\r\n");
		}
	}
	if (StateL == 2)
	{
		if (ClientL.connected())
		{
			SUPER.update();
		}
		else
		{
			StateL = 3;
		}
	}
	if (StateL == 3)
	{
		ClientL.stop();
		StateL = 1;
		DEBUGLOG("Disconnected...\r\n");
	}
}

/**
 * @brief Callback handler function.
 * 
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t * payload) {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (opcode == OpCodes::Ping)
	{
#ifdef SHOW_FUNC_NAMES
		DEBUGLOG("\r\n");
		DEBUGLOG(__PRETTY_FUNCTION__);
		DEBUGLOG("\r\n");
		DEBUGLOG("Ping...\r\n");
#endif // SHOW_FUNC_NAMES
		SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
	}
	else if (opcode == OpCodes::Stop)
	{
		Robko01.stop_motors();

		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::Disable)
	{
		Robko01.disable_motors();

		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::Enable)
	{
		Robko01.enable_motors();

		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::Clear)
	{
		Robko01.clear_motors();

		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::MoveRelative)
	{
		// If it is not enabled, do not execute.
		if (Robko01.motors_enabled() == false)
		{
			SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);
			return;
		}
		// If it is move, do not execute the command.
		if (MotorState_g != 0)
		{
			uint8_t m_payloadResponse[1] = { MotorState_g };
			SUPER.send_raw_response(opcode, StatusCodes::Busy, m_payloadResponse, 1);
			return;
		}

		// TODO: Move to function.
		size_t DataLengthL = sizeof(JointPosition_t);
		for (uint8_t index = 0; index < DataLengthL; index++)
		{
			MoveRelative_g.Buffer[index] = payload[index];
		}

		// Set motion data.
		Robko01.move_relative(MoveRelative_g.Value);
		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::MoveAbsolute)
	{
		// If it is not enabled, do not execute.
		if (Robko01.motors_enabled() == false)
		{
			// Respond with error.
			SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);

			// Exit
			return;
		}
		// If it is move, do not execute the command.
		if (MotorState_g != 0)
		{
			// Respond with busy.
			uint8_t m_payloadResponse[1];
			m_payloadResponse[0] = MotorState_g;
			SUPER.send_raw_response(opcode, StatusCodes::Busy, m_payloadResponse, 1);
			
			// Exit
			return;
		}

		// Extract motion data.
		size_t DataLengthL = sizeof(JointPosition_t);
		for (uint8_t index = 0; index < DataLengthL; index++)
		{
			MoveAbsolute_g.Buffer[index] = payload[index];
		}

		// Set motion data.
		Robko01.move_absolute(MoveAbsolute_g.Value);

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::DO)
	{
		// Set port A.
		Robko01.set_port_a(payload[0]);

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::DI)
	{
		uint8_t m_payloadResponse[1];
		m_payloadResponse[0] = Robko01.get_port_a();

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, m_payloadResponse, 1);
	}
	else if (opcode == OpCodes::IsMoving)
	{
#ifdef SHOW_FUNC_NAMES_S
		DEBUGLOG("\r\n");
		DEBUGLOG(__PRETTY_FUNCTION__);
		DEBUGLOG("\r\n");
		DEBUGLOG("IsMoveing...\r\n");
#endif // SHOW_FUNC_NAMES
		uint8_t m_payloadResponse[1];
		m_payloadResponse[0] = MotorState_g;

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, m_payloadResponse, 1);
	}
	else if (opcode == OpCodes::CurrentPosition)
	{
		CurrentPositions_g.Value = Robko01.get_position();

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, CurrentPositions_g.Buffer, sizeof(JointPosition_t));
	}
	else if (opcode == OpCodes::MoveSpeed)
	{
		// If it is not enabled, do not execute.
		if (Robko01.motors_enabled() == false)
		{
			SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);
			return;
		}
		

		for (uint8_t index = 0; index < size; index++)
		{
			MoveSpeed_g.Buffer[index] = payload[index];
		}
		
		// Set motion data.
		Robko01.move_speed(MoveSpeed_g.Value);
		
		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::SetRobotID)
	{
		// TODO: Write to I2C EEPROM.
		//for (uint8_t index = 0; index < DataLengthL; index++)
		//{
		//	Motion.Buffer[index] = m_payloadRequest[index];
		//}
		
		SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
	}
	else if (opcode == OpCodes::GetRobotID)
	{
		// TODO: Read from I2C EEPROM.
		//for (uint8_t index = 0; index < DataLengthL; index++)
		//{
		//	m_payloadRequest[index] = Motion.Buffer[index];
		//}
		
		SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
	}
}

#pragma endregion

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ESP8266)
// ESP8266
	DEBUGLOG("Flash chip size: %u\r\n", ESP.getFlashChipRealSize());
#endif

	DEBUGLOG("Sketch size: %u\r\n", ESP.getSketchSize());
	DEBUGLOG("Free flash space: %u\r\n", ESP.getFreeSketchSpace());
	DEBUGLOG("Free heap: %d\r\n", ESP.getFreeHeap());
	DEBUGLOG("Firmware version: %d\r\n", ESP_FW_VERSION);
	DEBUGLOG("SDK version: %s\r\n", ESP.getSdkVersion());
	DEBUGLOG("MAC address: %s\r\n", WiFi.macAddress().c_str());
	DEBUGLOG("\r\n");
}

#pragma endregion