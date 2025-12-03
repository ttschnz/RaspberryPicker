/**
 * Controller.cpp
 * 
 * Main controller implementation for the Raspberry Picker robot.
 * Manages the overall state machine and coordinates basket and gripper operations.
 * Implements various programs for automated raspberry picking, sorting, and basket management.
 */

#include "Controller.h"

#include "Basket/Basket.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"

#include "Gripper/Gripper.h"
#include "Gripper/GripperStepper.h"
/**
 * Constructor - creates a controller with specified initial state.
 * @param state Initial state for the controller
 */
Controller::Controller(State state)
{
    Controller(state, nullptr);
}
/**
 * Constructor - creates a controller with state and interface.
 * @param state Initial state (PROGRAM state defaults to IDLE)
 * @param interface Pointer to InterfaceMaster for communication
 */
Controller::Controller(State state, InterfaceMaster *interface)
{
    if (state == State::PROGRAM)
    {
        state = State::IDLE; // default to idle for safety
    }
    this->basket_controller = nullptr;
    this->gripper_controller = nullptr;
    this->interface = interface;
    this->set_state(state);
}

/**
 * Constructor - creates a controller with a specific program to execute.
 * @param program Program to be set for execution
 */
Controller::Controller(Program program)
{
    this->set_program(program);
}

/**
 * Gets the currently selected program.
 * @return Current program enum value
 */
Controller::Program Controller::get_program()
{
    return this->program;
}

/**
 * Gets the current controller state.
 * @return Current state enum value (IDLE, MANUAL, or PROGRAM)
 */
Controller::State Controller::get_state()
{
    return this->state;
}

/**
 * Sets the program to be executed.
 * Automatically transitions controller to PROGRAM state and notifies interface.
 * @param program Program to execute
 */
void Controller::set_program(Controller::Program program)
{
    this->set_state(Controller::State::PROGRAM);
    this->program = program;
    if (this->interface != nullptr)
    {
        this->interface->send_state("controller.program", this->serialize_program(this->get_program()));
    }
}

/**
 * Sets the controller state.
 * Notifies interface of state change if interface is available.
 * @param state New state to set (IDLE, MANUAL, or PROGRAM)
 */
void Controller::set_state(Controller::State state)
{
    this->state = state;
    if (this->interface != nullptr)
    {
        this->interface->send_state("controller.state", this->serialize_state(this->get_state()));
    }
}

/**
 * Adds references to basket and gripper controllers for coordinated operation.
 * @param basket_controller Pointer to basket controller
 * @param gripper_controller Pointer to gripper controller
 */
void Controller::add_controllers(BasketController *basket_controller, GripperController *gripper_controller)
{
    this->basket_controller = basket_controller;
    this->gripper_controller = gripper_controller;
}

/**
 * Adds reference to interface master for communication.
 * @param interface Pointer to InterfaceMaster
 */
void Controller::add_interface(InterfaceMaster *interface)
{
    this->interface = interface;
}

/**
 * Executes the close gripper program.
 * Attempts to close gripper in stages (LARGE -> SMALL -> LIMIT) until a raspberry
 * is detected. Measures color/ripeness and sets sorting mechanism accordingly.
 * If raspberry is unripe, releases it immediately.
 */
void Controller::run_close()
{

    // Attempt to close gripper at large size position
    GripperStepper::RaspberrySize size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_LARGE);

    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));

    // If no raspberry detected, try small size position
    if (size == GripperStepper::RaspberrySize::UNKNOWN)
    {
        size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_SMALL);
    }

    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));

    // If still no raspberry detected, close to limit switch
    if (size == GripperStepper::RaspberrySize::UNKNOWN)
    {
        size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_LIMIT);
    }

    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));

    // Measure color to determine ripeness
    bool is_ripe = this->gripper_controller->is_ripe();
    this->interface->send_state("gripper.raspberry_ripeness", is_ripe ? "RIPE" : "UNRIPE");

    // If unripe, release raspberry and exit
    if (!is_ripe)
    {
        this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
        return;
    }

    // Set sorting mechanism to appropriate position based on size
    switch (size)
    {
    case GripperStepper::RaspberrySize::LARGE:
        this->basket_controller->set_sorting(BasketSorter::SortingState::LARGE);
        break;
    case GripperStepper::RaspberrySize::SMALL:
        this->basket_controller->set_sorting(BasketSorter::SortingState::SMALL);
        break;
    case GripperStepper::RaspberrySize::UNKNOWN:
        // No raspberry detected even at limit switch - abort and reopen
        this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
        break;
    }
}

/**
 * Executes the release gripper program.
 * Opens gripper, increments the appropriate basket counter, and resets sorting to idle.
 */
void Controller::run_release()
{
    this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
    if (this->basket_controller->increment_counter() == false)
    {
        Serial.println((String) "cannot increment counter on sorting state " + BasketSorter::serialize_sorting_state(this->basket_controller->sorting_state));
    }
    this->basket_controller->set_sorting(BasketSorter::SortingState::IDLE);
}

/**
 * Executes the drop/empty basket program.
 * Opens basket door, resets fill counters, waits, then closes door.
 */
void Controller::run_drop()
{
    this->basket_controller->set_door(BasketDoor::DoorState::OPEN);
    this->basket_controller->reset_counter(false);
    delay(BasketDoor::delay_ms);
    this->basket_controller->set_door(BasketDoor::DoorState::CLOSED);
}

/**
 * Executes the reset program.
 * Resets all components to their initial positions and clears counters.
 */
void Controller::run_reset()
{
    this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_LIMIT);
    this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
    this->basket_controller->set_door(BasketDoor::DoorState::CLOSED);
    this->basket_controller->set_sorting(BasketSorter::SortingState::IDLE);
    this->basket_controller->reset_counter(true);
}

/**
 * Executes the continuous color measurement program.
 * Continuously closes gripper to small position, measures color, and reports values.
 * Runs until serial input is received. Used for color sensor calibration and testing.
 */
void Controller::run_measure_color()
{
    int desired_steps_halfopen = GripperStepper::mm_to_steps(32);
    while (Serial.available() == 0)
    {
        // Close gripper to small position
        this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_SMALL);
        
        // Measure raw RGB values
        RAW_RGB rgb_raw = this->gripper_controller->color_sensor->measure_rgb_raw();
        
        // Get current plate distance
        int current_position_step = this->gripper_controller->plate_stepper->currentPosition();
        int plate_distance = GripperStepper::steps_to_mm(current_position_step);
        
        // Report measurements
        Serial.println((String) "raw_value:" + rgb_raw.r + "/" + rgb_raw.g + "/" + rgb_raw.b + "/" + rgb_raw.noise + "/" + plate_distance);
        
        // Move to half-open position for next measurement
        this->gripper_controller->plate_stepper->setSpeed(GripperStepper::speed);
        this->gripper_controller->plate_stepper->moveTo(desired_steps_halfopen);
        while (this->gripper_controller->plate_stepper->isRunning())
        {
            this->gripper_controller->plate_stepper->run();
        }
        delay(100);
    }
}

/**
 * Executes Program 1 - Complete automated picking sequence.
 * 
 * This program performs the full raspberry picking cycle:
 * 1. Closes gripper progressively (LARGE -> SMALL -> LIMIT) to detect size
 * 2. Measures color/ripeness
 * 3. If unripe, releases and exits
 * 4. If ripe, sets sorting mechanism based on size
 * 5. Waits for user to pick raspberry (monitors pressure plate)
 * 6. Opens gripper, increments counter, resets sorting
 * 
 * Note: For UNKNOWN size (no pressure detected), assumes small size and
 * doesn't wait for pressure plate release.
 */
void Controller::run_pgm1()
{
    
    // Progressively close gripper until raspberry size is detected
    GripperStepper::RaspberrySize size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_LARGE);

    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));

    if (size == GripperStepper::RaspberrySize::UNKNOWN)
    {
        size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_SMALL);
    }

    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));

    if (size == GripperStepper::RaspberrySize::UNKNOWN)
    {
        size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_LIMIT);
    }

    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));

    // Detect ripeness using color sensor
    bool is_ripe = this->gripper_controller->is_ripe();
    this->interface->send_state("gripper.raspberry_ripeness", is_ripe ? "RIPE" : "UNRIPE");

    if (!is_ripe)
    {
        this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
        return;
    }

    // Set sorting mechanism to appropriate position
    // For unknown size: assume small (reached limit switch without detecting raspberry)
    switch (size)
    {
    case GripperStepper::RaspberrySize::LARGE:
        this->basket_controller->set_sorting(BasketSorter::SortingState::LARGE);
        break;
    case GripperStepper::RaspberrySize::UNKNOWN: 
    case GripperStepper::RaspberrySize::SMALL:
        this->basket_controller->set_sorting(BasketSorter::SortingState::SMALL);
        break;
    }

    // Wait for user to pick the raspberry
    // Exit when pressure plate loses contact or timeout reached
    // For UNKNOWN size, skip pressure monitoring (never detected contact)
    int delayed_time_ms = 0;
    bool berry_is_touching = true;
    do {
        if (size != GripperStepper::RaspberrySize::UNKNOWN){
            berry_is_touching = this->gripper_controller->limit_switch_pressure->is_touching();
        }
        delay(100);
        delayed_time_ms+=100;

    } while (delayed_time_ms < GripperController::picking_delay_ms && berry_is_touching);

    // Complete the cycle: open gripper, increment counter, reset sorting
    this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
    this->basket_controller->increment_counter();
    this->basket_controller->set_sorting(BasketSorter::SortingState::IDLE);
}


/**
 * Executes Program 2 - Empty basket program.
 * Opens basket door, resets counters, waits for contents to fall, then closes door.
 */
void Controller::run_pgm2()
{
    this->basket_controller->set_door(BasketDoor::DoorState::OPEN);
    this->basket_controller->reset_counter(false);
    delay(BasketDoor::delay_ms);
    this->basket_controller->set_door(BasketDoor::DoorState::CLOSED);
}

/**
 * Converts program enum to string representation.
 * @param program Program enum value
 * @return String name of the program
 */
const char *Controller::serialize_program(Program program)
{
    int idx = static_cast<int>(program);

    const char *program_strings[] = {
        "CLOSE_GRIPPER",
        "RELEASE_GRIPPER",
        "EMPTY_BASKET",
        "RESET",
        "MEASURE_COLOR",
        "PROGRAM_1",
        "PROGRAM_2",
    };
    return program_strings[idx];
}
/**
 * Converts string to program enum.
 * @param program String representation of program
 * @param out_program Pointer to store resulting Program enum
 * @return true if string matched a valid program, false otherwise
 */
bool Controller::deserialize_program(String program, Controller::Program *out_program)
{
    bool matched = true;
    if (program == "CLOSE_GRIPPER")
    {
        *out_program = Controller::Program::CLOSE_GRIPPER;
    }
    else if (program == "RELEASE_GRIPPER")
    {
        *out_program = Controller::Program::RELEASE_GRIPPER;
    }
    else if (program == "EMPTY_BASKET")
    {
        *out_program = Controller::Program::EMPTY_BASKET;
    }
    else if (program == "RESET")
    {
        *out_program = Controller::Program::RESET;
    }
    else if (program == "MEASURE_COLOR")
    {
        *out_program = Controller::Program::MEASURE_COLOR;
    }
    else if (program == "PROGRAM_1")
    {
        *out_program = Controller::Program::PROGRAM_1;
    }
    else if (program == "PROGRAM_2")
    {
        *out_program = Controller::Program::PROGRAM_2;
    }
    else
    {
        matched = false;
    }
    return matched;
}

/**
 * Converts state enum to string representation.
 * @param state State enum value
 * @return String name of the state
 */
const char *Controller::serialize_state(State state)
{
    int idx = static_cast<int>(state);
    const char *state_strings[3] = {
        "MANUAL",
        "IDLE",
        "PROGRAM"};
    return state_strings[idx];
}

/**
 * Converts string to state enum.
 * @param state String representation of state
 * @param out_state Pointer to store resulting State enum
 * @return true if string matched a valid state, false otherwise
 */
bool Controller::deserialize_state(String state, Controller::State *out_state)
{
    bool matched = true;
    if (state == "MANUAL")
    {
        *out_state = Controller::State::MANUAL;
    }
    else if (state == "IDLE")
    {
        *out_state = Controller::State::IDLE;
    }
    else if (state == "PROGRAM")
    {
        *out_state = Controller::State::PROGRAM;
    }
    else
    {
        matched = false;
    }
    return matched;
}
