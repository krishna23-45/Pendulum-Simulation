# Pendulum Simulation Project

Welcome to the Pendulum Simulation project! This project is a physics-based pendulum simulation with real-time visualizations of various physical properties, including position, kinetic energy, potential energy, and total energy. The simulation is interactive and visually engaging, making it a great educational tool.

## Features

- **Pendulum Motion:** The pendulum swings back and forth, simulating realistic physics.
- **Graph Visualizations:** Real-time graphs for position, kinetic energy, potential energy, and total energy.
- **Interactive Control:** Adjust the pendulum's starting position by right-clicking and dragging the bob.
- **Trace Path:** The pendulum's path is traced with randomly generated colors for a vibrant effect.
- **Real-Time Calculations:** Displays real-time values for physical properties.

## Getting Started

### Prerequisites
Ensure you have the following installed:
- C++ Compiler (e.g., g++)
*   [SDL2](https://www.libsdl.org/)
*   [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)

## Project Structure

*   `pf.cc`: Main C++ source file.
*   `Makefile`: Build the project.
*   `src/`: Source code directory.
    *   `include/`: Header files.
    *   `lib/`: SDL related

### Installation
1. Clone the repository:
   ```bash
   git clone 
   ```
## Building

This project uses `make` for building. To build the project, run the following command in your terminal:

2. Navigate to the project directory:
   ```bash
   cd pendulum-simulation
   ```
3. Compile the code:
   ```bash
    make
   ```
4. Run the executable:
   ```bash
   ./pendulum_simulation
   ```
5. To clean up the build artifacts
   ```bash
    make clean
   ```
## Usage
- **Running the Simulation:** Launch the program, and the pendulum will start swinging.
- **Changing Initial Position:** Right-click and drag the bob to set a new starting position.
- **Viewing Graphs:** Graphs for position and energy values update in real time.

## Code Overview

### `main` Function
- The entry point of the program.
- Calls the `updatePendulum` function for physics calculations.

### `updatePendulum`
- Calculates pendulum motion based on physics principles.
- Skips calculations if the user is dragging the bob.

### `manageTracePath`
- Manages the colorful trace path of the pendulum.
- Automatically removes older points to prevent overflow.

### `updateGraph`
- Updates position, kinetic energy, and potential energy data.
- Ensures data structures stay within a manageable size.

### `renderGraph`
- Renders real-time graphs for position and energy values.

### `Pendulum` Struct
- Represents the pendulum's state, including angle, angular velocity, and length.

## Future Improvements
- Code refactoring for better organization.
- Enhanced graphing features and improved visual aesthetics.
- Additional physics models for extended educational purposes.

## Contributing
Contributions are welcome! Feel free to fork the repository and submit a pull request with your improvements.

## License
This project is licensed under the MIT License.

## Contact
If you have any questions or suggestions, feel free to reach out or open an issue.

## Demo Video
Check out the project demo video on YouTube: [Demo Video Link]

Thank you for exploring this project! Consider starring the repository if you found it helpful.

