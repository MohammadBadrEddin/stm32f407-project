# STM32F407VG CAN Node Project

## Project Description
This project implements a **CAN bus node** on the STM32F407VG Discovery board in combination with the **Agonia expansion board** (developed by Frankfurt University of Applied Sciences), which provides a simple CAN bus interface through a D-SUB connector.  
The software was developed using **Atollic TrueSTUDIO**.

The main goal was to configure and handle CAN communication with specific message IDs, using an interrupt-driven approach.

### Hardware References

This project is based on the following hardware:

- **STM32F407VG Discovery Board**  
  [Product page](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)  

  [![STM32F407VG Discovery](assets/image.PF252419.en.feature-description-include-personalized-no-cpn-large.avif)

- **STM32F407VG Microcontroller**  
  [Product page](https://www.st.com/en/microcontrollers-microprocessors/stm32f407vg.html)
---

## Assignment Requirements
- **Target hardware:** STM32F407VG Discovery board + Agonia CAN expansion board  
- **System clock:** 168 MHz (configured using the ST-provided clock configuration tool)  
- **CAN bus configuration:**  
  - Data rate: 500 kBd  
  - 11-bit standard identifiers  
- **Node IDs:**  
  - ID #1: `0x31`  
  - ID #2: `0x32`  

---

## Functional Implementation
1. **Receive messages via CAN (interrupt-driven):**  
   - Message 1: ID = `0x23`, DLC = 4  
   - Message 2: ID = `0x56`, DLC = 5  

2. **User button interrupt (blue button on Discovery board):**  
   - On button press, send a CAN message using ID `0x31`  
   - This message contains the **number of received messages with ID `0x23`**  
   - Value is stored in data bytes 1–4 (DLC = 4)  

3. **Message processing for ID `0x56`:**  
   - Determine the **smallest and largest data byte** from the received message  
   - Send a new CAN message with ID `0x32`, DLC = 2  
   - Byte 0 = smallest value, Byte 1 = largest value  

4. **Status LED (red LED on Discovery board):**  
   - Toggle every time a CAN message is transmitted  

---

## Tools & Environment
- **IDE:** Atollic TrueSTUDIO (Eclipse-based)  
- **MCU:** STM32F407VG  
- **Expansion Board:** Agonia board (FRAUAS)  
- **Bus speed:** 500 kBd  
- **Language:** C  

---

## How to Build & Run
1. Open the project in **Atollic TrueSTUDIO**.  
2. Make sure the STM32F407VG Discovery board is connected with the Agonia expansion board.  
3. Compile and flash the firmware to the board.  
4. Connect the board to a CAN bus via the D-SUB connector.  
5. Use a CAN Test Tool (e.g. CANoe) to send and receive messages.  

---

## Demo Behavior
- The node automatically processes CAN messages with IDs `0x23` and `0x56`.  
- The **user button** triggers sending the count of received `0x23` messages.  
- For messages with ID `0x56`, the node responds with the smallest and largest data byte.  
- The **red LED toggles** every time a CAN message is transmitted.  
