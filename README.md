# Rhythm-Password

## Overview
This project introduces a novel security system that integrates gesture and rhythm recognition for enhanced access control. Utilizing the Arduino Nano 33 BLE Sense, it features a dual-layered security approach combining a gesture-based lock with a rhythm-based lock, leveraging machine learning for improved accuracy and user experience.

### Authors
- Pranit Sehgal - *School of Computing and Augmented Intelligence, Arizona State University* - pvsehgal@asu.edu

## Features
- **Gesture Lock**: Uses the Arduino's APDS-9960 sensor for gesture detection, allowing hand gestures as a unique access code.
- **Rhythm Lock**: Incorporates keystroke dynamics analyzed through machine learning, adding a layer of security by recognizing the unique rhythm of key presses.
- **Machine Learning**: Employs TensorFlow/Keras Sequential models optimized for embedded systems, providing real-time processing capabilities.

## Getting Started

### Prerequisites
- Arduino Nano 33 BLE Sense
- 4x4 Keypad
- Basic understanding of Arduino programming and machine learning principles.

### Installation
1. Clone the repository to your local machine.
2. Ensure you have the Arduino IDE and TensorFlow installed.
3. Connect the Arduino Nano 33 BLE Sense to your computer and upload the provided sketch.

## Usage
To use the security system, perform a gesture or enter a PIN on the keypad. The system will analyze the input in real-time to authenticate access based on the trained machine learning model.

## Contributing
We welcome contributions from the community, whether it's improving the code, addressing issues, or adding new features. Please create a pull request or open an issue to discuss your ideas!

## License
This project is licensed under the MIT License - see the LICENSE.md file for details.

## Acknowledgments
- Association for the Advancement of Artificial Intelligence (www.aaai.org) for supporting this research.
- All contributors who have helped in refining and testing the system.

## Future Work
- Simplify the rhythm lock setup process based on user feedback.
- Explore the integration of AI-driven rhythm assignment for ease of use.
- Extend the application scope to include residential security, personal device access, and corporate security solutions.

## Contact
For any queries or further information, please reach out to Pranit Sehgal at pvsehgal@asu.edu.
