![MicrowriterC](https://github.com/user-attachments/assets/31648309-af10-4bf5-b139-43d44c9ffa23)

# ⌨️🖥️ Microwriter

**A modern ESP32-P4 recreation of the Microwriter chorded text-entry system.**

Microwriter is a functional reinterpretation of the portable writing machine created by **Cy Endfield and Chris Rainey** in the late 1970s and commercialized in the early 1980s.

Instead of a conventional QWERTY keyboard, letters are entered by pressing **combinations — or chords — of five keys** positioned for the fingers of one hand.

This modern version uses a **DFRobot FireBeetle 2 ESP32-P4**, six physical buttons, a 16×2 RGB LCD, Wi-Fi, and microSD storage.

Text can be:

* ✍️ entered using five-key chords
* 🖥️ displayed on the RGB LCD
* 📜 scrolled automatically as it grows
* 📡 retrieved through a Wi-Fi web interface
* 💾 saved to a microSD card
* 🎓 practiced with the included browser-based chord trainer

---

## ✨ Features

* ⌨️ Five-key chorded text entry
* 👍 Dedicated thumb key
* 🖐️ One key for each finger
* ⌫ Dedicated delete button
* 🔤 A–Z alphabet support
* ✒️ Space and basic punctuation
* 🖥️ 16×2 RGB LCD output
* 📜 Automatic text scrolling
* 📡 ESP32 Wi-Fi Access Point
* 🌐 Built-in web interface
* 📊 Character and word counters
* 💾 microSD text storage
* 🎓 Interactive browser-based Microwriter trainer
* 🖨️ Custom 3D-printable enclosure
* 📜 MIT licensed

---

# 🕰️ The original Microwriter

The original **Microwriter** was developed by filmmaker and inventor **Cy Endfield**, working with engineer **Chris Rainey**.

Its goal was unusual: enable useful text entry with one hand and without a conventional keyboard.

The device used a chorded keyboard:

```text
THUMB   INDEX   MIDDLE   RING   PINKY
  ●       ●       ●       ●      ●
```

Rather than assigning one letter to each key, characters were produced by pressing several keys simultaneously.

Conceptually:

```text
Individual keys
      ↓
Combination / chord
      ↓
Letter
      ↓
Text
```

This allowed a very small device to encode an entire alphabet with only a handful of buttons.

The original hardware also included a display, memory, rechargeable batteries, and interfaces for transferring or printing text.

Historical references:

* **[Centre for Computing History — Microwriter MW4](https://www.computinghistory.org.uk/det/36099/Microwriter-MW4/)**
* **[Science Museum Group — Experimental and pre-production Microwriters](https://collection.sciencemuseumgroup.org.uk/objects/co38161/experimental-and-pre-production-models-of-the-microwriter-typewriter)**
* **[HCI Museum — Microwriter](https://interfacemuseum.com/exhibits/microwriter/)**

---

# 🧠 How this version works

```text
┌─────────────────────────┐
│      Five chord keys    │
│                         │
│ T   I   M   R   P       │
└────────────┬────────────┘
             │
             │ GPIO
             ▼
┌─────────────────────────┐
│ FireBeetle 2 ESP32-P4   │
│                         │
│ • Detect chord          │
│ • Map chord → character │
│ • Maintain text buffer  │
│ • Drive LCD             │
│ • Run Wi-Fi AP          │
│ • Run HTTP server       │
│ • Write microSD files   │
└──────┬───────────┬──────┘
       │           │
       ▼           ▼
┌────────────┐ ┌───────────────┐
│ 16×2 LCD   │ │ Wi-Fi Browser │
│            │ │               │
│ Live text  │ │ Read / save   │
└────────────┘ └───────┬───────┘
                       │
                       ▼
                 ┌───────────┐
                 │ microSD   │
                 │ .txt file │
                 └───────────┘
```

The firmware continuously reads the state of the five chord buttons.

Each key becomes either:

```text
0 = released
1 = pressed
```

The five values are concatenated into a binary-looking chord.

For example:

```text
01100
```

maps to:

```text
A
```

The character is then appended to the current text buffer and displayed on the LCD.

---

# ⌨️ Chord system

The five chord inputs are processed in this order:

```text
Thumb → Index → Middle → Ring → Pinky
```

Example mappings:

| Character | Chord   |
| --------- | ------- |
| A         | `01100` |
| B         | `00111` |
| C         | `10100` |
| E         | `01000` |
| H         | `10001` |
| O         | `00100` |
| S         | `00010` |
| T         | `01010` |
| U         | `00001` |
| Z         | `10101` |
| Space     | `10000` |
| `.`       | `01110` |
| `-`       | `01101` |
| `,`       | `01011` |
| `'`       | `11101` |

The firmware contains mappings for the complete alphabet.

---

# 🎓 Interactive chord trainer

Learning a chorded alphabet is initially less intuitive than using QWERTY, so the repository includes a small browser game for practicing the key combinations.

**[▶️ Open the Microwriter Chord Trainer](https://ronibandini.github.io/Microwriter/html/index.html)**

The trainer shows a target character and lets you select:

```text
THUMB
INDEX
MIDDLE
RING
PINKY
```

It tracks:

* ✅ correct answers
* 🔢 attempts
* 📊 accuracy

The trainer source is included under:

```text
html/
├── index.html
└── readme.txt
```

---

# 🧰 Hardware

| Component                         | Purpose               |
| --------------------------------- | --------------------- |
| **DFRobot FireBeetle 2 ESP32-P4** | Main controller       |
| **DFRobot RGB LCD1602 V2**        | Text display          |
| **5 × push buttons**              | Chord keyboard        |
| **1 × push button**               | Delete key            |
| **microSD card**                  | Text storage          |
| **4 × M3 screws**                 | Enclosure assembly    |
| Jumper wires                      | Connections           |
| USB-C cable                       | Programming and power |
| 3D-printed enclosure              | Physical case         |

### FireBeetle 2 ESP32-P4

**[DFRobot FireBeetle 2 ESP32-P4](https://www.dfrobot.com/product-2950.html)**

### RGB LCD

**[DFRobot RGB LCD1602 V2](https://www.dfrobot.com/product-1609.html)**

---

# 🔌 Wiring

## Chord buttons

The current firmware defines:

| Function | GPIO |
| -------- | ---: |
| Thumb    | `31` |
| Index    | `34` |
| Middle   | `20` |
| Ring     | `36` |
| Pinky    | `35` |
| Delete   | `21` |

The source configuration is:

```cpp
int pinThumb = 31;
int pinIndex = 34;
int pinMiddle = 20;
int pinRing = 36;
int pinPinky = 35;
int pinDelete = 21;
```

Each button connects between its GPIO and **GND**.

The firmware uses:

```cpp
INPUT_PULLUP
```

so no external pull-up resistors are required.

> ℹ️ Some earlier project documentation lists a slightly different GPIO assignment. The table above follows the **current GitHub firmware**.

---

## LCD

The DFRobot RGB LCD1602 communicates over I²C.

Connect:

| LCD | ESP32-P4 |
| --- | -------- |
| VCC | VCC      |
| GND | GND      |
| SDA | SDA      |
| SCL | SCL      |

The firmware initializes the display as:

```cpp
DFRobot_RGBLCD1602 lcd(
    0x2D,
    16,
    2
);
```

The original build uses a red LCD backlight:

```cpp
lcd.setRGB(255, 0, 0);
```

---

# 📜 Display behavior

The LCD has:

```text
16 columns × 2 rows
```

so it can display 32 characters at once.

When the current text contains 32 characters or fewer, the first sixteen characters appear on the first row and the remaining characters on the second.

Once the text exceeds 32 characters, the display automatically shows the latest portion:

```text
...previous text
              ↓
┌────────────────┐
│latest 16 chars │
│latest 16 chars │
└────────────────┘
```

The complete text remains in memory even though only the latest 32 characters are visible.

---

# ⌫ Delete key

A sixth physical button acts as a dedicated delete key.

Rapid presses are counted inside a short time window:

```cpp
const unsigned long doubleClickTime = 400;
```

If the button is pressed several times rapidly, the firmware removes the corresponding number of characters from the end of the text.

For example:

```text
HELLO WORLD
```

three quick delete presses become:

```text
HELLO WO
```

---

# 📡 Wi-Fi Access Point

The Microwriter creates its **own Wi-Fi network**.

It does not need to connect to your home router.

The default configuration in the firmware is:

```cpp
const char* ap_ssid = "Microwriter";
const char* ap_password = "12345678";
```

> 🔐 Change the default password before using the device in a public environment.

The Access Point starts automatically after approximately **20 seconds of inactivity**.

When it starts, the LCD displays something similar to:

```text
AP: Microwriter
192.168.4.1
```

Connect a phone, tablet, or computer to:

```text
SSID: Microwriter
```

and open the IP address shown on the display.

---

# 🌐 Web interface

The ESP32-P4 runs an HTTP server on:

```text
port 80
```

The main route:

```text
/
```

displays a web interface containing:

* 📝 current text
* 🔢 character count
* 📊 word count
* 💾 microSD status
* 🔄 refresh button
* 💾 save button

This provides a simple way to retrieve the text without connecting the Microwriter to another computer over USB.

Conceptually:

```text
Microwriter
     │
     │ Wi-Fi Access Point
     ▼
Phone / Laptop
     │
     ▼
http://DEVICE_IP
     │
     ▼
Copy text
```

---

# 💾 microSD storage

The FireBeetle 2 ESP32-P4 onboard TF/microSD slot is accessed through:

```cpp
SD_MMC
```

The firmware initializes it in 1-bit mode:

```cpp
SD_MMC.begin("/sdcard", true);
```

From the web interface, press:

```text
💾 Save to SD
```

The ESP32 writes the complete current text to a file similar to:

```text
/microwriter_123456.txt
```

The numeric part is based on the ESP32 runtime counter returned by:

```cpp
millis()
```

The file contains the complete text buffer.

---

# 🚀 Installation

## 1. Clone the repository

```bash
git clone https://github.com/ronibandini/Microwriter.git
cd Microwriter
```

---

## 2. Install ESP32 support

Install a current ESP32 Arduino core that supports the **ESP32-P4**.

Select an ESP32-P4-compatible board configuration in Arduino IDE.

The source was developed for:

```text
ESP32P4 Dev Module
```

---

## 3. Install the LCD library

Install:

**[DFRobot_RGBLCD1602](https://github.com/DFRobot/DFRobot_RGBLCD1602)**

The firmware includes:

```cpp
#include "DFRobot_RGBLCD1602.h"
```

---

## 4. Configure the Access Point

Before uploading, change:

```cpp
const char* ap_ssid = "Microwriter";
const char* ap_password = "12345678";
```

Example:

```cpp
const char* ap_ssid = "MyMicrowriter";
const char* ap_password = "YOUR_PASSWORD";
```

---

## 5. Upload the firmware

Open:

```text
microwriter5.ino
```

Compile and upload it to the FireBeetle 2 ESP32-P4.

The serial monitor uses:

```text
115200 baud
```

---

# 🎮 Using the Microwriter

### 1. Power on

The LCD displays:

```text
Microwriter 1.0
Roni Bandini
```

The firmware then initializes the microSD card.

### 2. Type

Press combinations of the five chord keys.

For example:

```text
01100 → A
```

The letter appears immediately on the LCD.

### 3. Correct text

Use the dedicated delete button.

### 4. Retrieve text

Stop typing for roughly 20 seconds.

The device starts its Wi-Fi Access Point and displays its IP address.

Connect to the network and open that address in a browser.

### 5. Save

Press:

```text
💾 Save to SD
```

to write the current text to a `.txt` file.

---

# 📁 Repository structure

```text
Microwriter/
├── html/
│   ├── index.html
│   └── readme.txt
├── images/
├── LICENSE
├── README.md
└── microwriter5.ino
```

### `microwriter5.ino`

Main ESP32-P4 firmware implementing:

* chord detection
* character mapping
* LCD output
* deletion
* text buffering
* Wi-Fi Access Point
* HTTP server
* web interface
* microSD storage

### `html/index.html`

Standalone browser game for learning the Microwriter chord alphabet.

---

# 🖨️ 3D enclosure

The modern enclosure was designed for the FireBeetle, LCD, and six physical buttons.

The downloadable design contains three STL files:

```text
Cuerpo.stl
Tapa.stl
base.stl
```

Recommended printing configuration:

```text
Material: PLA
Supports: Yes
```

### Cults3D

**[Microwriter — 3D printable enclosure](https://cults3d.com/en/3d-model/gadget/microwriter)**

The Cults3D page also includes photos of the completed build and internal assembly.

---

# 🎥 Demo

Watch the modern Microwriter in operation:

**[▶️ Microwriter demo on YouTube](https://www.youtube.com/shorts/yYJEvJfksPs)**

---

# 🔬 Ideas for extending the project

1. **🔠 Add modes for numbers and symbols** — reproduce more of the original Microwriter's modal input system instead of limiting chords primarily to letters and basic punctuation.

2. **💾 Build a document manager** — browse, reopen, rename, download, and delete saved microSD documents directly from the web interface.

3. **⌨️ Add USB HID output** — allow the Microwriter to act as a chorded USB keyboard for a computer, tablet, or other host.

---

# 📰 External references

Microwriter has been documented and referenced outside GitHub.

## 🗞️ Independent / community coverage

### DFRobot Maker Community China

**[DF创客周刊 — November 21, 2025](https://mc.dfrobot.com.cn/thread-398703-1-1.html)**

DFRobot's Chinese Maker Community included the project in its weekly collection of noteworthy maker projects.

The feature describes the modern Microwriter as a recreation of Cy Endfield's portable writing device and highlights:

* the FireBeetle ESP32-P4
* six-button interface
* 16×2 RGB LCD
* 3D-printed enclosure
* Wi-Fi text retrieval
* microSD storage

The article references the Hackster build tutorial as its source.

---

# 🛠️ Project tutorials

## Hackster.io

**[Making a Microwriter](https://www.hackster.io/roni-bandini/making-a-microwriter-15087e)**

Complete English-language build tutorial published November 14, 2025.

It includes:

* history of the original Microwriter
* parts list
* enclosure design
* wiring
* Arduino setup
* chord operation
* Wi-Fi text retrieval
* microSD storage
* demo
* source-code links
* 3D files

---

## Medium

**[Fabricando un oscuro dispositivo de los 70 para escribir en el camino](https://bandini.medium.com/fabricando-un-oscuro-dispositivo-de-los-70-para-escribir-en-el-camino-674d4c4f8be9)**

Spanish-language article by Roni Bandini about the history of the Microwriter and the construction of this modern ESP32-P4 version.

This article is also linked from the GitHub repository's **About** section.

---

# 🖨️ 3D model

## Cults3D

**[Microwriter](https://cults3d.com/en/3d-model/gadget/microwriter)**

The downloadable enclosure package includes the modern case, photographs of the finished build, and STL files.

Published November 14, 2025.

---

# 🎥 Video

## YouTube

**[Microwriter demo](https://www.youtube.com/shorts/yYJEvJfksPs)**

Short demonstration of the completed hardware and chorded text-entry system.

---

# 🎓 Companion project

## GitHub Pages

**[Microwriter 5-finger chord alphabet trainer](https://ronibandini.github.io/Microwriter/html/index.html)**

Interactive web application included with this repository for learning and practicing the five-key alphabet.

---

# 📚 Historical references

For readers interested in the original device:

### Centre for Computing History

**[Microwriter MW4](https://www.computinghistory.org.uk/det/36099/Microwriter-MW4/)**

Museum documentation for the original Microwriter MW4, including its chord keyboard, LCD, processor, memory, batteries, and interfaces.

---

### Science Museum Group

**[Experimental and pre-production models of the Microwriter typewriter](https://collection.sciencemuseumgroup.org.uk/objects/co38161/experimental-and-pre-production-models-of-the-microwriter-typewriter)**

The Science Museum collection includes original experimental and pre-production devices donated by Cy Endfield.

---

### HCI Museum

**[Microwriter](https://interfacemuseum.com/exhibits/microwriter/)**

Historical overview of the Microwriter as an early portable text-entry and human-computer-interface experiment.

---

# 🔗 You may also be interested in...

Other projects by **Roni Bandini** involving alternative writing systems, portable computing, and unconventional interfaces.

## ⌨️📚 Kindle Typewriter

**Turn a jailbroken Kindle Paperwhite into a minimalist networked typewriter with a Raspberry Pi and thermal printer.**

Another experiment in stripping writing technology back to a deliberately constrained physical interface.

**[github.com/ronibandini/Kindle-Typewriter](https://github.com/ronibandini/Kindle-Typewriter)**

---

## 📚🤖 Gemini for Kindle

**Run Google Gemini directly from a jailbroken Kindle using BusyBox, Bash, KTerm, and the Gemini API.**

Like Microwriter, it repurposes hardware intended for one interaction model and turns it into something quite different.

**[github.com/ronibandini/GeminiForKindle](https://github.com/ronibandini/GeminiForKindle)**

---

## 🗂️🤖 PunchedCards

**Punched-card recognition using computer vision, Edge Impulse, and LattePanda IOTA.**

Another project revisiting an obsolete computing interface with contemporary hardware and software.

**[github.com/ronibandini/PunchedCards](https://github.com/ronibandini/PunchedCards)**

---

# 🔐 Network note

The firmware creates a Wi-Fi Access Point and serves the current text through an HTTP server.

The default source contains:

```cpp
const char* ap_password = "12345678";
```

Change this password before normal use.

The web interface uses plain HTTP and does not implement user authentication beyond the Wi-Fi password itself.

Avoid using it for sensitive text on an untrusted network.

---

# 📜 License

Microwriter is released under the **MIT License**.

See [`LICENSE`](LICENSE) for details.

---

# 👤 Author

**Roni Bandini**

Maker, AI developer, electronic artist and writer.

* 🐙 GitHub: **[@ronibandini](https://github.com/ronibandini)**
* 📸 Instagram: **[@ronibandini](https://www.instagram.com/ronibandini/)**
* 🐦 X: **[@RoniBandini](https://x.com/RoniBandini)**
* ✍️ Medium: **[bandini.medium.com](https://bandini.medium.com/)**
* 🛠️ Hackster: **[Roni Bandini](https://www.hackster.io/roni-bandini)**

Contributions, forks, chord-layout experiments, alternative enclosures, and writing-interface adaptations are welcome.
