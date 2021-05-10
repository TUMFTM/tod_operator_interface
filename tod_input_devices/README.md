# tod_input_devices  
This package handles multiple input devices for the TUM ToD Software Stack.

## Supported input devices  
* **USB Input Devices:** Works with various usb input devices like Joysticks, XBOX-Controllers, Steering Wheels, etc
* **Virtual Input Devices:** Opens a virtual joystick on the screen.

# Dependencies  
  * C++17 
  * ROS Packages: see `package.xml`
  * Other Libraries
    * [Qt](https://www.qt.io/) >= 5.9 (Widgets QuickWidgets): 
      ```
      sudo apt-get install qt5-default qml-module-qtquick2 qtpositioning5-dev
      ```

# Documentation
## InputDevice
Directly reads from hardware or virtual input device and publishes a [sensor_msgs/Joy](http://docs.ros.org/en/melodic/api/sensor_msgs/html/msg/Joy.html).


**Published Topics:**
  * `/Operator/InputDevices/joystick` ([sensor_msgs/Joy](http://docs.ros.org/en/melodic/api/sensor_msgs/html/msg/Joy.html)) Contains the state of buttons and axes of the current input device.

**Services:**  
  * `/Operator/InputDevices/change_input_device` ([tod_msgs/InputDevice](https://github.com/TUMFTM/tod_common/blob/master/tod_msgs/srv/InputDevice.srv)) Service to switch between different input devices on runtime. This service can be called from the `tod_manager`. The service call requires the path to the desired `.yaml` file (see [Configuration](#configuration)).

# Quick Start
## Configuration
The folder _/config_ contains multiple predefined configurations for input devices. These can be set in _/launch/tod_input_devices.launch_ `ConfigFile` or at Runtime using the Service Call. To create a new configuration for a USB input device proceed with the following steps:

1. Copy the `template.yaml` under `config/` to <desired_name>.yaml in the same folder.
2. In _/launch/tod_input_devices.launch_ set `ConfigMode` to `true` and `ConfigFile` to your <desired_name>.yaml.  
3. Start the Launch File: `roslaunch tod_input_devices tod_input_devices.launch`  
4. Press a Button -> Assign the number showing up in the Terminal to the desired action in <desired_name>.yaml  
5. Move a Axis -> Assign the number showing up in the Terminal to the desired action in <desired_name>.yaml  
6. Save <desired_name>.yaml and set `ConfigMode` to `false`

You can use this configuration now by setting `ConfigFile` to your <desired_name>.yaml in _/launch/tod_input_devices.launch_
