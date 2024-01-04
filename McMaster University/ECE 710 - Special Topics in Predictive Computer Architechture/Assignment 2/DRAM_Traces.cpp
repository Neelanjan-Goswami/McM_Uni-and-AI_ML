#include <iostream>
#include <string>
#include <getopt.h>
#include <random>
#include <fstream>
#include <iomanip> // For std::setw
#include <sstream> // For std::hex

int bank_size = 8;
int row_size = 32768;
int col_size = 1024/64;

std::ofstream output;

// To form address according to the mapping type
std::string address_form(int bank, int row, int col, std::string address_mapping) {
    int address_width = 28;
    std::string b = std::bitset<3>(bank).to_string();
    std::string r = std::bitset<15>(row).to_string();
    std::string c = std::bitset<10>(col).to_string(); // Adjust to match the address width.

    std::string ans = "000000";
    if (address_mapping == "row-bnk-col")
        ans = r + b + c + ans;
    else if (address_mapping == "row-col-bnk")
        ans = r + c + b + ans;
    else if (address_mapping == "bnk-col-row")
        ans = b + c + r + ans;

    return ans.substr(0, address_width);
}


// To generate sequential addresses
void generate_seq_address(int count, int total_request, int bank, int row, int col, std::string address_mapping, int type, int address_width) {
    for (int b = bank; b < bank_size; b++) {
        for (int r = row; r < row_size; r++) {
            for (int c = col; c < col_size; c++) {
                std::string address = address_form(b, r, c, address_mapping);
                
                // Convert binary to hexadecimal using stringstream
                std::stringstream hexStream;
                hexStream << "0x" << std::hex << std::setw((address_width + 3) / 4) << std::setfill('0') << std::stoi(address, 0, 2);
                std::string hex = hexStream.str();

                if (type == 0)
                    output << hex << " READ 0" << std::endl;
                else if (type == 1)
                    output << hex << " WRITE 0" << std::endl;
                else {
                    if (count % 2 == 0)
                        output << hex << " READ 0" << std::endl;
                    else
                        output << hex << " WRITE 0" << std::endl;
                }

                count++;
                if (count == total_request)
                    return;
            }
        }
    }
}



void generate_random_address(int count, int total_request, std::string address_mapping, int type, int address_width) {
    srand(time(NULL));

    while (count < total_request) {
        int bank = rand() % bank_size;
        int row = rand() % row_size;
        int col = rand() % col_size;
        std::string address = address_form(bank, row, col, address_mapping);

        // Convert binary to hexadecimal using stringstream
        std::stringstream hexStream;
        hexStream << "0x" << std::hex << std::setw((address_width + 3) / 4) << std::setfill('0') << std::stoi(address, 0, 2);
        std::string hex = hexStream.str();

        if (type == 0)
            output << hex << " READ 0" << std::endl;
        else if (type == 1)
            output << hex << " WRITE 0" << std::endl;
        else {
            if (count % 2 == 0)
                output << hex << " READ 0" << std::endl;
            else
                output << hex << " WRITE 0" << std::endl;
        }
        count++;
    }
}

void generate_row_hit_address(int count, int total_request, std::string address_mapping, int type, int address_width) {
    srand(time(NULL));

    int bank = rand() % bank_size;
    int row = rand() % row_size;

    while (count < total_request) {
        int col = rand() % col_size;
        std::string address = address_form(bank, row, col, address_mapping);

        // Convert binary to hexadecimal using stringstream
        std::stringstream hexStream;
        hexStream << "0x" << std::hex << std::setw((address_width + 3) / 4) << std::setfill('0') << std::stoi(address, 0, 2);
        std::string hex = hexStream.str();

        if (type == 0)
            output << hex << " READ 0" << std::endl;
        else if (type == 1)
            output << hex << " WRITE 0" << std::endl;
        else {
            if (count % 2 == 0)
                output << hex << " READ 0" << std::endl;
            else
                output << hex << " WRITE 0" << std::endl;
        }
        count++;
    }
}

void generate_row_conflict_address(int count, int total_request, std::string address_mapping, int type, int address_width) {
    srand(time(NULL));

    int row = rand() % row_size;
    int row_prev = rand() % row_size;

    while (count < total_request) {
        int col = rand() % col_size;
        int bank = rand() % bank_size;
        while (row == row_prev) {
            row = rand() % row_size;
        }
        row_prev = row;
        std::string address = address_form(bank, row, col, address_mapping);

        // Convert binary to hexadecimal using stringstream
        std::stringstream hexStream;
        hexStream << "0x" << std::hex << std::setw((address_width + 3) / 4) << std::setfill('0') << std::stoi(address, 0, 2);
        std::string hex = hexStream.str();

        if (type == 0)
            output << hex << " READ 0" << std::endl;
        else if (type == 1)
            output << hex << " WRITE 0" << std::endl;
        else {
            if (count % 2 == 0)
                output << hex << " READ 0" << std::endl;
            else
                output << hex << " WRITE 0" << std::endl;
        }
        count++;
    }
}

void dram(int address_width, std::string address_mapping, int total_request, std::string address_pattern, std::string type_pattern) {
    output.open("dram_traces.txt");

    int type = 0;

    if (type_pattern == "R")
        type = 0;
    else if (type_pattern == "W")
        type = 1;
    else
        type = 2;

    if (address_pattern == "sequential") {
        int count = 0;
        int bank = 0;
        int row = 0;
        int col = 0;
        generate_seq_address(count, total_request, bank, row, col, address_mapping, type, address_width);
    } else if (address_pattern == "random") {
        int count = 0;
        generate_random_address(count, total_request, address_mapping, type, address_width);
    } else if (address_pattern == "row-hit") {
        int count = 0;
        generate_row_hit_address(count, total_request, address_mapping, type, address_width);
    } else if (address_pattern == "row-conflict") {
        int count = 0;
        generate_row_conflict_address(count, total_request, address_mapping, type, address_width);
    }

    output.close();
}

int main(int argc, char* argv[]) {
    int address_width = 28; // Default value
    int num_requests = 10000; // Default value
    std::string address_pattern = "sequential"; // Default value
    std::string type_pattern = "RW"; // Default value
    std::string address_mapping = "row-bnk-col"; // Default value

    // Parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "w:m:n:p:t:")) != -1) {
        switch (opt) {
            case 'w':
                address_width = std::stoi(optarg);
                break;
            case 'm':
                address_mapping = optarg;
                break;
            case 'n':
                num_requests = std::stoi(optarg);
                break;
            case 'p':
                address_pattern = optarg;
                break;
            case 't':
                type_pattern = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " -w [address_width] -m [address_mapping] -n [num_requests] -p [address_pattern] -t [type_pattern]" << std::endl;
                return 1;
        }
    }

    std::cout<<address_width<<std::endl;
	std::cout<<address_mapping<<std::endl;
	std::cout<<num_requests<<std::endl;
	std::cout<<address_pattern<<std::endl;
	std::cout<<type_pattern<<std::endl;

    dram(address_width, address_mapping, num_requests, address_pattern, type_pattern);

    return 0;
}
