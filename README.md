# WIP
## TinyC2

TinyC2 is a lightweight, modular Command and Control (C2) framework that serves as both a learning tool and a practical foundation for red teaming. Designed for ease of use and flexibility, it is adaptable and reusable, allowing for quick customization and extension for real-world operations.

It's designed with simplicity (KISS principle) and stealth in mind, this project serves as a starting point for red teams and startups to customize and expand it based on their needs.

The framework consists of:

1. **Server**: The **command controller** that allows the operator (Player) to send instructions to connected clients and view their responses.
2. **Client (Implant)**: A **lightweight agent** deployed on target systems, designed to execute commands and report back to the server.
3. **Player (Operator)**: The **red teamer** who runs the server, issues commands via the server interface, and manages engagements.
4. **Modular Design**: A streamlined architecture that makes it extremely easy to add, remove, or update functionality through simple modules.


---


## Features
- **Minimal Footprint**: Lightweight and unobtrusive design.
- **Operator-Focused**: Simple interface for red teamers to control engagements.
- **Stealthy Communication**: Simple TCP-based protocol for controlled environments.
- **Modular Command Handlers**: Effortlessly extend functionality with reusable modules.
- **Cross-Platform**: Works on Linux and Windows.
- **Customizable**: Designed as a template for adaptation to diverse operational needs.

---

## Installation Prerequisites
- GCC or a compatible C compiler
- Networking libraries (`arpa/inet.h`, `winsock2.h` on Windows)

## Usage 

1. **Run the Server**: Start the server to listen for incoming connections from clients.
  `./server`
3. **Deploy the Implant**: Execute the client binary (implant) on the target system. This connects back to the server.
4. **Issue Commands**: Use the server interface to issue commands to connected clients and review responses. 

## Adding Modules :

Adding new functionality to the client is designed to be quick and easy. For example :

---

### 1. Create Your Module File

Create a new `.c` file (e.g., `modules/hello_module.c`) for your custom command handler:
```c
#include <string.h>

// Function to handle the "hello" command
void handle_hello(char *response) {
    strcpy(response, "Hello, world! Module executed successfully.\n");
}
```

---

### 2. Add the Module to the Client

Include your module in the `client.c` file by adding this line at the top:
```c
#include "modules/hello_module.c"
```

---

### 3. Register the Command

Add the new command to the command dispatcher in `client.c`:
```c
if (strncmp(buffer, "hello", 5) == 0) {
    handle_hello(response);
}
```

---

### 4. Rebuild the Client

Recompile the client to include the new module:
```bash
gcc -o client client.c
```

Your new module is now fully functional and ready for use!

---

### Example Workflow

1. **Server Input**: 
   ```bash
   Enter command: hello
   ```

2. **Client Response**: 
   ```
   Hello, world! Module executed successfully.
   ```

That’s it! Adding modules is as simple as writing a function and registering it.


### **Roadmap**
### Proof Of Concept :
- [x] Working POC 
#### **v1.0: Foundational Features**
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

