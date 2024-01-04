/* 
 
	32k rows = 15 bits
	1kcolumns = 10 bits
	8 banks = 3 bits
	offset = 6 bits ??
 
 . Sequential 
 . Random 
 . All row hit 
 . All row conflicts
 
 . All R 
 . All W 
 . Switching: R, W, R, W..
 
 o RW-RNK-BNK-CL
 o RW-CL-RNK-BNK 
 o RNK-BNK-CL-RW
 */

#include <iostream>
#include <string>
#include <getopt.h>
#include <random>
#include <vector>
#include <fstream>

using namespace std;


//output stream
ofstream output;

int bank_size = 8;
int row_size = 32768;
int col_size = 1024/64;

// function to print in the output file traces.txt
void print(string hex, int type, int count)
{
	switch(type)
	{
		case 0:
			hex = hex + " "+"READ 0";
			break;
		case 1:
			hex = hex + " "+"WRITE 0";
			break;
		case 2:
			if(count % 2 == 0)
				hex = hex + " "+"READ 0";
			else
				hex = hex + " "+"WRITE 0";
			break;
	}			
	
	output<<hex<<endl;
	//std::cout<<hex<<std::endl;
}

// To convert binary string into hexadecimal string
string binary_to_hex(string str)
{
	str = "00"+str;
	int n = str.size();
	string rest = "0x";
	//int temp = str.substr(i,2);
	for(int i = 0;i<n;i = i+4)
	{
		string tmp = str.substr(i,4);
		
		if (!tmp.compare("0000"))
		{
			rest = rest + "0";
		}
		else if (!tmp.compare("0001"))
		{
			rest = rest + "1";
		}
		else if (!tmp.compare("0010"))
		{
			rest = rest + "2";
		}
		else if (!tmp.compare("0011"))
		{
			rest = rest + "3";
		}
		else if (!tmp.compare("0100"))
		{
			rest = rest + "4";
		}
		else if (!tmp.compare("0101"))
		{
			rest = rest + "5";
		}
		else if (!tmp.compare("0110"))
		{
			rest = rest + "6";
		}
		else if (!tmp.compare("0111"))
		{
			rest = rest + "7";
		}
		else if (!tmp.compare("1000"))
		{
			rest = rest + "8";
		}
		else if (!tmp.compare("1001"))
		{
			rest = rest + "9";
		}
		else if (!tmp.compare("1010"))
		{
			rest = rest + "a";
		}
		else if (!tmp.compare("1011"))
		{
			rest = rest + "b";
		}
		else if (!tmp.compare("1100"))
		{
			rest = rest + "c";
		}
		else if (!tmp.compare("1101"))
		{
			rest = rest + "d";
		}
		else if (!tmp.compare("1110"))
		{
			rest = rest + "e";
		}
		else if (!tmp.compare("1111"))
		{
			rest = rest + "f";
		}
		else
		{
			continue;
		}
	}
	return rest;
}

// To form address according to the mapping type
string formulate(int bank, int row, int col, string address_mapping)
{
	string b = bitset<3>(bank).to_string();
	string r = bitset<15>(row).to_string();
	string c = bitset<10>(col).to_string();
	
	string ans = "000000";
	if(address_mapping == "row-bnk-col")
		ans = r+b+c+ans;
	else if(address_mapping == "row-col-bnk")
		ans = r+c+b+ans;
	else if(address_mapping == "bnk-col-row")
		ans = b+c+r+ans;
		
	return ans.substr(0);
}

// To generate sequential adddresses
void generate_seq_address(int count,int total_request, int bank, int row, int col, string address_mapping, int type)
{
	
	for(int b=bank;b<bank_size;b++)
	{
		for(int r=row;r<row_size;r++)
		{
			for(int c=col;c<col_size;c++)
			{
				
				string address = formulate(b,r,c, address_mapping);
				string hex = binary_to_hex(address);
				//std::cout<<address<<"---->"<<hex<<std::endl;
				
				print(hex,type,count);
				
				count++;
				if(count == total_request)
					return;
			}
		}
	}
}

void create(int address_width, string address_mapping, int total_request, string address_pattern, string type_pattern)
{
	output.open("traces.txt");
	
	int type = 0;
	
	if(type_pattern == "R")
			type = 0;
		else if(type_pattern == "W")
			type = 1;
		else
			type = 2;
	
	if(address_pattern == "sequential")
	{
		int count = 0;
		
		int bank = 0;
		int row = 0;
		int col = 0;
		
		generate_seq_address(count,total_request,bank,row,col,address_mapping,type);	
	}
	else if(address_pattern == "random")
	{
		srand (time(NULL));
		int count = 0;
		while(count < total_request)
		{
			int bank = rand() % bank_size;
			int row = rand() % row_size;
			int col = rand() % col_size;
			string address = formulate(bank, row, col, address_mapping);
			string hex = binary_to_hex(address);
			print(hex,type,count);
			count++;
		}
	}
	else if(address_pattern == "row-hit")
	{
		srand (time(NULL));
		
		int bank = rand() % bank_size;
		int row = rand() % row_size;
		int count = 0;
		
		while(count < total_request)
		{			
			int col = rand() % col_size;
			string address = formulate(bank, row, col, address_mapping);
			string hex = binary_to_hex(address);
			print(hex,type,count);
			count++;
		}
	}
	else if(address_pattern == "row-conflict")
	{
		srand (time(NULL));
		
		
		int row = rand() % row_size;
		int row_prev = rand() % row_size;
		int count = 0;
		
		while(count < total_request)
		{			
			int col = rand() % col_size;
			int bank = rand() % bank_size;
			while(row == row_prev)
			{
				row = rand() % row_size;
			}
			row_prev = row;
			string address = formulate(bank, row, col, address_mapping);
			string hex = binary_to_hex(address);
			print(hex,type,count);
			count++;
		}
	}
	output.close();
}

int main(int argc, char *argv[])
{
	//create(28,"row-bnk-col",10000,"sequential","R");
	//create(28,"row-bnk-col",10000,"random","R");
	//create(28,"row-bnk-col",10000,"row-conflict","R");
	//create(28,"row-bnk-col",10000,"row-hit","R");
	//create(28,"row-bnk-col",10000,"row-hit","RW");

	string flag_width;
	int address_width;
	
	string flag_mapping;
	string address_mapping;	//RW-RNK-BNK-CL, RW-CL-RNK-BNK, RNK-BNK-CL-RW
	
	string flag_request;
	int total_request;
	
	string flag_pattern;
	string address_pattern;	// Sequential, Random, All row hit, All row conflicts
	
	string flag_type;
	string type_pattern;  // R, W or RW
	
	//std::cout<<argc<<endl<<argv[0]<<std::endl;
	if(argc != 11)
	{
		std::cout<<"Invalid parameters";
		return 0;
	}
		
	flag_width = argv[1];
    address_width = atoi(argv[2]);
    
	flag_mapping = argv[3];
	address_mapping = argv[4];
	
	flag_request = argv[5];
	total_request = atoi(argv[6]);
	
	flag_pattern = argv[7];
	address_pattern = argv[8];
	
	flag_type = argv[9];
	type_pattern = argv[10];
	
	if(flag_width != "-w" || flag_mapping != "-m" || flag_request != "-n" || flag_pattern != "-p" || flag_type != "-t")
	{
		std::cout<<"Invalid flags";
		return 0;
	}
	
	
	
	std::cout<<address_width<<std::endl;
	std::cout<<address_mapping<<std::endl;
	std::cout<<total_request<<std::endl;
	std::cout<<address_pattern<<std::endl;
	std::cout<<type_pattern<<std::endl;
	
	create(address_width, address_mapping, total_request, address_pattern, type_pattern);
	
	
	return 0;
}

