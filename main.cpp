#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Union for fuel type – only one fuel type can be used per car.
union FuelType {
    char petrol;
    char diesel;
    char electric;
};

// Struct for car details
struct Car {
    int ID;
    string model;
    double dailyRate;
    bool isAvailable;
    FuelType fuel;
};

// Struct for customer details
struct Customer {
    string name;
    string licenseNumber;
    int rentedCarID; // -1 means no car is currently rented
};

// Global arrays to store car and customer data
const int MAX_CARS = 5;
const int MAX_CUSTOMERS = 10;
Car cars[MAX_CARS];
Customer customers[MAX_CUSTOMERS];
int customerCount = 0;

// ----------------- PRICE CALCULATION FUNCTIONS -----------------

// Standard price calculation: rate × number of days
double standardRate(double rate, int days) {
    return rate * days;
}

// Discounted price calculation: 10% off
double discountRate(double rate, int days) {
    return (rate * days) * 0.9;
}

// ----------------- FILE I/O: SAVE AND LOAD CARS -----------------

// Save car data to a file
void saveCarsToFile() {
    ofstream file("cars.txt");
    for (int i = 0; i < MAX_CARS; i++) {
        char fuelChar = cars[i].fuel.petrol ?: cars[i].fuel.diesel ?: cars[i].fuel.electric;
        file << cars[i].ID << " " << cars[i].model << " " << cars[i].dailyRate << " "
             << cars[i].isAvailable << " " << fuelChar << "\n";
    }
    file.close();
}

// Load car data from a file
void loadCarsFromFile() {
    ifstream file("cars.txt");
    for (int i = 0; i < MAX_CARS && file >> cars[i].ID; i++) {
        string fuelStr;
        file >> cars[i].model >> cars[i].dailyRate >> cars[i].isAvailable >> fuelStr;
        char fuelChar = fuelStr[0];
        if (fuelChar == 'P') cars[i].fuel.petrol = 'P';
        else if (fuelChar == 'D') cars[i].fuel.diesel = 'D';
        else if (fuelChar == 'E') cars[i].fuel.electric = 'E';
    }
    file.close();
}

// ----------------- FILE I/O: SAVE AND LOAD CUSTOMERS -----------------

// Save customer data to a file
void saveCustomersToFile() {
    ofstream file("customers.txt");
    file << customerCount << "\n"; // Save the count first
    for (int i = 0; i < customerCount; i++) {
        file << customers[i].name << " " << customers[i].licenseNumber << " " << customers[i].rentedCarID << "\n";
    }
    file.close();
}

// Load customer data from a file
void loadCustomersFromFile() {
    ifstream file("customers.txt");
    if (!file) return; // Skip if file doesn't exist
    file >> customerCount;
    for (int i = 0; i < customerCount; i++) {
        file >> customers[i].name >> customers[i].licenseNumber >> customers[i].rentedCarID;
    }
    file.close();
}

// ----------------- USER FUNCTIONS -----------------

// Add a new customer to the system
void addCustomer() {
    if (customerCount >= MAX_CUSTOMERS) {
        cout << "Customer limit reached.\n";
        return;
    }
    Customer& c = customers[customerCount];
    cout << "Enter name: ";
    cin >> c.name;
    cout << "Enter license number: ";
    cin >> c.licenseNumber;
    c.rentedCarID = -1;
    customerCount++;
}

// Display all available cars
void displayAvailableCars() {
    cout << "Available Cars:\n";
    for (int i = 0; i < MAX_CARS; i++) {
        if (cars[i].isAvailable) {
            char fuel = cars[i].fuel.petrol ?: cars[i].fuel.diesel ?: cars[i].fuel.electric;
            cout << "ID: " << cars[i].ID
                 << ", Model: " << cars[i].model
                 << ", Rate: R" << cars[i].dailyRate
                 << ", Fuel: " << (fuel == 'P' ? "Petrol" : fuel == 'D' ? "Diesel" : fuel == 'E' ? "Electric" : "Unknown")
                 << "\n";
        }
    }
}

// Rent a car to a customer (only one rental allowed at a time)
void rentCar() {
    string license;
    int carID;
    cout << "Enter license number: ";
    cin >> license;

    for (int i = 0; i < customerCount; i++) {
        if (customers[i].licenseNumber == license) {
            // Check if customer already rented a car
            if (customers[i].rentedCarID != -1) {
                cout << "You already have a rented car (Car ID: "
                     << customers[i].rentedCarID
                     << "). Please return it before renting another.\n";
                return;
            }

            displayAvailableCars();
            cout << "Enter Car ID to rent: ";
            cin >> carID;
            for (int j = 0; j < MAX_CARS; j++) {
                if (cars[j].ID == carID && cars[j].isAvailable) {
                    customers[i].rentedCarID = carID;
                    cars[j].isAvailable = false;
                    cout << "Car rented successfully.\n";
                    return;
                }
            }
            cout << "Car not available.\n";
            return;
        }
    }
    cout << "Customer not found.\n";
}

// Return a car and calculate total price using selected function pointer
void returnCar(double (*priceCalc)(double, int)) {
    string license;
    int days;
    cout << "Enter license number: ";
    cin >> license;

    for (int i = 0; i < customerCount; i++) {
        if (customers[i].licenseNumber == license && customers[i].rentedCarID != -1) {
            cout << "Enter number of rental days: ";
            cin >> days;
            for (int j = 0; j < MAX_CARS; j++) {
                if (cars[j].ID == customers[i].rentedCarID) {
                    double total = priceCalc(cars[j].dailyRate, days);
                    cout << "Total cost: R" << total << "\n";
                    cars[j].isAvailable = true;
                    customers[i].rentedCarID = -1;
                    return;
                }
            }
        }
    }
    cout << "Customer not found or no car rented.\n";
}

// Display all customers and their current rental status
void displayCustomers() {
    cout << "Customers:\n";
    for (int i = 0; i < customerCount; i++) {
        cout << "Name: " << customers[i].name
             << ", License: " << customers[i].licenseNumber
             << ", Rented Car ID: " << (customers[i].rentedCarID == -1 ? "None" : to_string(customers[i].rentedCarID))
             << "\n";
    }
}

// ----------------- MAIN MENU -----------------

// User menu for navigation
void menu() {
    int choice;
    loadCarsFromFile();        // Load car data at startup
    loadCustomersFromFile();   // Load customer data at startup

    do {
        cout << "\nCar Rental System Menu:\n";
        cout << "1. Add Customer\n";
        cout << "2. Rent Car\n";
        cout << "3. Return Car (Standard Rate)\n";
        cout << "4. Return Car (Discounted Rate)\n";
        cout << "5. Show Available Cars\n";
        cout << "6. Show Customers\n";
        cout << "7. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addCustomer(); break;
            case 2: rentCar(); break;
            case 3: returnCar(standardRate); break;
            case 4: returnCar(discountRate); break;
            case 5: displayAvailableCars(); break;
            case 6: displayCustomers(); break;
            case 7:
                saveCarsToFile();         // Save cars before exiting
                saveCustomersToFile();    // Save customers before exiting
                cout << "Exiting...\n";
                break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 7);
}

// ----------------- MAIN FUNCTION -----------------

int main() {
    // Default car list (used only if no file exists)
    cars[0] = {1, "Lamborghini_Aventador", 25000.0, true, {'P'}};
    cars[1] = {2, "Bugatti_Chiron", 30000.0, true, {'P'}};
    cars[2] = {3, "Tesla_Model3", 1800.0, true, {'E'}};
    cars[3] = {4, "Volkswagen_Polo", 850.0, true, {'P'}};
    cars[4] = {5, "BMW_X5", 2000.0, true, {'D'}};

    menu(); // Start the program
    return 0;
}

