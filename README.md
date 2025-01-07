# QT Camera Application

## Overview
This project is a camera application developed using **Qt**, a powerful C++ framework for building cross-platform applications. The application demonstrates how to use the Qt Multimedia module to interact with a camera, capture images, and display live video feeds.

---

## Features
- **Live Camera Feed:** Stream and display the video feed from a connected camera.
- **Image Capture:** Capture images with a single click.
- **Save Captured Images:** Save the captured images to a specified location.
- **User Interface:** Simple and responsive UI for camera controls.

---

## Technologies Used
- **Programming Language:** C++
- **Framework:** Qt (Qt Multimedia module)
- **Qt Version:** 6.x or higher

---

## Prerequisites
- **Qt Development Environment:** Install Qt Creator or any IDE that supports Qt.
- **Qt Multimedia Module:** Ensure it is included in your Qt installation.
- **C++ Compiler:** Compatible with your Qt version.

---

## Installation

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/subtlemortal/QTCamera.git
   cd QTCamera
   ```

2. **Open the Project in Qt Creator:**
   - Launch Qt Creator.
   - Open the `QTCamera.pro` file from the project directory.

3. **Configure the Build Environment:**
   - Select the appropriate kit for your platform (e.g., Desktop Qt 6.x GCC 64-bit).

4. **Build the Project:**
   - Click on the **Build** button to compile the application.

5. **Run the Application:**
   - Click the **Run** button in Qt Creator to launch the camera application.

---

## Usage

1. **Start the Application:**
   - The live camera feed will be displayed in the main window.

2. **Capture an Image:**
   - Click the **Capture** button to take a snapshot of the current feed.

3. **Save Captured Images:**
   - Captured images are saved to the default or user-specified directory.

4. **Close the Application:**
   - Exit the application by clicking the **Close** button or the window's close icon.

---

## Project Structure
```
QTCamera/
|└── src/
|   |└── main.cpp          # Application entry point
|   |└── mainwindow.cpp   # Main window logic
|   |└── mainwindow.h     # Header for main window
|   |└── camera.cpp       # Camera interaction logic
|   |└── camera.h         # Header for camera logic
|└── ui/
|   |└── mainwindow.ui    # Qt Designer UI file
|└── resources/
|   |└── icons/          # Icons and assets
|└── QTCamera.pro        # Qt project configuration file
```

---

## Customization
- Modify the `camera.cpp` and `mainwindow.cpp` files to add new features or customize behavior.
- Update the UI using Qt Designer by editing the `mainwindow.ui` file.

---

## Dependencies
- **Qt Multimedia Module**: Required for camera and multimedia operations.
- **OpenCV (optional):** Can be integrated for advanced image processing (not included in the base project).

---

## Contributing
Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch for your feature or bugfix.
3. Commit your changes and open a pull request.

---
