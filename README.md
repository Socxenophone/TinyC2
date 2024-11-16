# TinyC2

TinyC2 is a simple, modular Command and Control framework written in C. Designed with simplicity (KISS principle) and stealth in mind, this project serves as a **template** for red teams and startups to customize and expand based on their needs.

The framework basically consists of:
1. **Server**: Command controller to interact with clients.
2. **Client**: Lightweight agent (implant) that executes commands and reports results.
3. **Modular Design**: Simplified dynamic extension of functionality through command handlers.

> **Disclaimer**: This project is for **educational and ethical penetration testing purposes only**. Unauthorized use is strictly prohibited.

---

## Features
- **Minimal Footprint**: Lightweight and unobtrusive design.
- **Modular Command Handlers**: Easily extendable with new functionality.
- **Stealthy Communication**: Simple TCP-based protocol for controlled environments.
- **Cross-Platform**: Compatible with Linux and Windows.
- **Template for Customization**: Adaptable to diverse operational needs.

---

## Installation

### Prerequisites
- GCC or a compatible C compiler
- Networking libraries (`arpa/inet.h`, `winsock2.h` on Windows)


---

### **Roadmap**

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

