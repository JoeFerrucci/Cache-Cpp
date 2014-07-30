

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iomanip>
using namespace std;

class line
{
	public:
		//int offset;
		unsigned int set;
		unsigned int tag;
		unsigned int address;
		unsigned int dirty;
		unsigned int word[8];
		//string word[8];
		//bool empty;
		line()
		{
		 set = 0;
		 tag = 0;
		 address = 0;
		 dirty = 0;
		 //empty = true;
		 for(int i = 0; i < 8; i++)
		 	word[i]= 0;
		}
};


vector<line> mem; //main memory

//Add the set to memory
int addmem(line set)
{
	bool inmem = false;
	//first is the tag is already existed in the mem
	for(int i = 0; i < mem.size(); i++)
	{
		if(set.tag == mem.at(i).tag)
		{
			mem.at(i) = set;
			inmem = true;
			return 0;
		}
	}
	
	if(inmem == false)
	{
		mem.push_back(set);
		return 0;
	}
	
}

//does the set exitst in the memory
bool exitstmem(int tag)
{
	for(int i = 0; i < mem.size(); i++)
	{
		if(tag == mem.at(i).tag)
			return true;
	}
	
	return false;
}

//get the set from mem
line getmem(int tag)
{
	for(int i = 0; i < mem.size(); i++)
	{
		if(tag == mem.at(i).tag)
			return mem.at(i);
	}
}

int main()
{
	ofstream ofile("dm-output.txt");
	ofile << "";
	ofile.close();
	
	string fline;
	line cache[32]; // 256 byte cache divided-by 8 byte line sizes, gives 32 entries (lines)
	int valid[32];
	for(int i = 0; i < 32; i++)
		valid[i] = 0;
	
	string inputf;
	cout << "Enter the input file" << endl;
	cin >> inputf;
	
	ifstream myfile(inputf.c_str());
	int writeprev = 0; // did the write the line before
	if(myfile.is_open())
	{
		while(getline (myfile,fline) )
		{
			string s = fline.substr(0,4);//getting the address    // breaking up the address on input
			string d = fline.substr(8,2);//getting the data   		// breaking up the input data
			string mode = fline.substr(5,2);			
			//cout << d << endl;
			unsigned int address = (unsigned int) strtol(s.c_str(), NULL, 16);
			unsigned int last8 = address & 0xFF; //getting the left 8 bit;
			unsigned int set = last8 >> 3;
			unsigned int tag = address >> 8;
			unsigned int offset = last8 & 0x7;
			unsigned int data = (unsigned int) strtol(d.c_str(), NULL, 16); // 16 bit address.
			
			if(mode == "FF") //Write operation
			{
				if(valid[set] == 0)
				{
					cache[set].address = address;
					cache[set].set = set;
					cache[set].tag = tag;
					cache[set].dirty = 1;
					cache[set].word[offset] = data;
					valid[set] = 1;
				}
				//first find it in the cache or if the set is empty
				else if(tag == cache[set].tag  ) 
				{
					cache[set].word[offset] = data;
					cache[set].dirty = 1;
					if(valid[set] == 0)
						valid[set] = 1;
				}
				
				else if(tag != cache[set].tag && valid[set] == 1) //not in the cache or the set is occupy by
				//something else
				{
					//first add the set to mem.
					addmem(cache[set]);
					if(exitstmem(tag))//if it exist in memory
					{
						cache[set] = getmem(tag);
						cache[set].word[offset] = data;
						cache[set].dirty = 1;
					}
					
					else
					{
						cache[set].address = address;
						cache[set].tag = tag;
						cache[set].set = set;
						cache[set].dirty = 1;
						cache[set].word[offset] = data;
					}
					
				}
			}
			
			else
			{
				ofstream ofile("dm-output.txt" , ios::app);
				//first find in cache
				if(cache[set].tag == tag)
				{
					int dataf = cache[set].word[offset];
					if(dataf != data && d != "")
					{
						dataf = data;
						cache[set].word[offset] = data;
					}
					ofile << hex << uppercase << setfill('0') << setw(2) << dataf;
					ofile << " " << 1 << " " << cache[set].dirty << endl;
					ofile.close();
				}
				
				else
				{
					int dirtyRead = cache[set].dirty;
					addmem(cache[set]);
					cache[set] = getmem(tag);
					cache[set].dirty = 0;
					int dataf = cache[set].word[offset];
					if(dataf != data && d != "")
					{
						dataf = data;
						cache[set].word[offset] = data;
					}
					ofile << hex << uppercase << setfill('0') << setw(2) <<   dataf;
					ofile << " " << 0 << " " << dirtyRead << endl;
					ofile.close();
				}
				
			}
			
		}
		myfile.close();
	}
	
	else 
		cout << "Unable to open file" << endl;
	
	return 0;
}
