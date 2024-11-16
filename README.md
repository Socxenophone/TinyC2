# WIP
## TinyC2
![](https://github.com/Socxenophone/TinyC2/blob/main/thumbnail.jpg) 


A lightweight, modular Command and Control (C2) framework, designed for both learning and practical red teaming. Built with the KISS principle and stealth in mind, it provides a flexible foundation that can be easily customized and extended for real-world operations or specific needs. 

---


## Features :

1. **Minimal Footprint**: Optimized for low resource usage and system impact.
2. **Operator-Focused**: Streamlined for efficiency in execution and management of operations.
3. **Stealthy Communication**: Custom TCP protocol with fallback channels (HTTP/DNS) for secure, discreet communication.
4. **Modular Command Handlers**: Easy-to-add modules for core functions, with a minimal base set.
5. **MSF Integration**: Seamless integration with Metasploit for exploits and payloads.
6. **Cross-Platform**: Supports both Linux and Windows systems.
7. **Customizable**: Adaptable for various operational needs and engagements.
8. **Secure Authentication**: Lightweight but secure access control for authorized users.
9. **Basic Agent Management**: Simple deployment and real-time status feedback for agents.

## Structure :

The framework consists of:

1. **Server**: The command controller that allows the operator (Player) to send instructions to connected clients and view their responses.
2. **Agent (Implant)**: A lightweight agent deployed on target systems, designed to execute commands and report back to the server.
3. **Player**: The client interface for the red teamer, used to run the command server, issue commands, and manage engagements.
4.**Modules**: A collection of interchangeable modules that provide various functionalities. Easily add, remove, update or write modules to extend the framework’s capabilities.

## Installation Prerequisites :

Grab the latest version from the release page. 

If you want to compile its yourself, you'll need :
- GCC or a compatible C compiler
- Networking libraries (`arpa/inet.h`, or `winsock2.h` on Windows)

## Usage 

1. **Run the Server**: Start the server to listen for incoming connections from clients.
  `./server`
3. **Deploy the Implant**: Execute the client binary (implant) on the target system. This connects back to the server.
4. **Issue Commands**: Use the server interface to issue commands to connected clients and review responses. 

## Adding Modules :

Adding modules is as simple as writing a function and registering it. For example :

#### 1. Create Your Module File

Create a new `.c` file (e.g., `modules/hello_module.c`) for your custom command handler:
```c
#include <string.h>

// Function to handle the "hello" command
void handle_hello(char *response) {
    strcpy(response, "Hello, world! Module executed successfully.\n");
}
```

#### 2. Add the Module to the Client

Include your module in the `client.c` file by adding this line at the top:
```c
#include "modules/hello_module.c"
```


#### 3. Register the Command

Add the new command to the command dispatcher in `client.c`:
```c
if (strncmp(buffer, "hello", 5) == 0) {
    handle_hello(response);
}
```


#### 4. Rebuild the Client

Recompile the client to include the new module:
```bash
gcc -o client client.c
```

Your new module is now fully functional and ready for use!


##### Example Workflow

1. **Server Input**: 
   ```bash
   Enter command: hello
   ```

2. **Client Response**: 
   ```
   Hello, world! Module executed successfully.
   ```

*v1.0: Foundational Features**
- [ ] Basic TCP-based communication between server and client.
- [ ] Command execution (e.g., `ls`, `exec`).
- [ ] Modular architecture with extendable command handlers.
- [ ] Stealthy, lightweight design for minimal detection.

#### **v2.0: Platform Independence and Security**
- [ ] Add Windows compatibility using `winsock2.h`.
- [ ] Implement encrypted communication (e.g., AES or TLS).
- [ ] Multi-client support with threading (server handles concurrent connections).
- [ ] Basic persistence module for implants.

#### **v3.0: Advanced Features**
- [ ] Dynamic module loading (e.g., `.so`/`.dll` files for extensibility).
- [ ] HTTP/S and DNS-based communication for covert channels.
- [ ] Command and response obfuscation to evade detection.
- [ ] Expand command library (e.g., file upload/download, lateral movement).

#### **Future Enhancements**
- [ ] Sandbox evasion and detection mechanisms.
- [ ] Integration with popular red-team tools like Cobalt Strike or Metasploit.
- [ ] Comprehensive logging and audit trail for ethical engagements.


>
> **Disclaimer**: This project is for **educational purposes only**. Unauthorized use is strictly prohibited. And I take no responsibility whatsoever 

