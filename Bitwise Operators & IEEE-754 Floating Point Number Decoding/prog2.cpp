//***************************************************************************
//
//  Cesar Caballero
//  z1944013
//  CSCI 463-MSTR
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//***************************************************************************

#include <iostream>
#include <iomanip>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

void printBinFloat(uint32_t x);

/**
 * @brief  Uses a read loop to call printBinFloat() function to print out extracted information from hexadecimal input numbers.
 * 
 * @return int 0 when run is succesful.
 */
int main()
{
    uint32_t x;
    //receive input and store into "x" as hex value
    while(cin >> std::hex >> x)
    {
        printBinFloat(x); // call function to print out x
    }

    return 0;
}

/**
 * @brief read hex numbers from std input and extract information such as
 *        binary equivalence, sign bit, exponent, significand.
 * 
 * @param x = unsigned 32 bit integer to hold the hex from input
 */
void printBinFloat(uint32_t x)
{
    int n; // used to keep track amount of zeros needed to print the in final line
    int sign; //keeps track of sign bit if negative or positive value
    int exponent = ((x & 0x7f800000) >> 23) - 127; //create exponent variable as int to be signed
    int counter = 0; //counter to keep track of bits being printed

    uint32_t y = 0x80000000; //variable to use mask for binary conversion
    uint32_t significand = (x & 0x007fffff); //mask that extracts significand to variable


    cout << std::hex << "0x" << std::setw(8) << std::setfill('0') << x << " = ";
    
    //loop individually through 32 bits
    for(int i = 1; i <= 32; ++i)
    {
        cout << (x & y ? '1':'0'); //print hex numbers as its binary equivalent
        counter++;//keep track of bits printed

        if((i % 4 == 0) && (counter != 32)) //once 4 digits are printed print a space
        {
            cout << " ";
        }
        y >>= 1; // shift mask 1 to right after each loop
    }
    cout <<endl;

    //use mask to check if number is positive or negative value and set the sign bit accordingly
    if(x & 0x80000000)
    { 
        sign = 1; //negative
    }
    else
        sign = 0; // positive

    cout << "sign: " << sign << endl; //print the sign of hex number


    cout << " exp: 0x" << std::setw(8) << std::setfill('0') << exponent; //print exponent saved to exponent variable
    cout << " (" << std::dec << exponent << ")" << endl; // print exponent but as decimal equivalent

    
    cout << " sig: 0x"  << std::setw(8) << std::setfill('0') << std::hex << significand << endl; //print significand as hex value

    //case if the exponent equals -127
    if(exponent == -127)
    {
        //if else statement prints positive or negative symbol when appropriate
        if(sign == 1)
        {
            cout << "-";
        }
        else 
            cout << "+";
        
        cout << "0" << endl; //print one "0" after the sign
    }

    //case when exponent is 128
    else if(exponent == 128)
    {
        //if else statement prints positive or negative symbol when appropriate
        if(sign == 1)
        {
            cout << "-";
        }
        else
            cout << "+";
        
        cout << "inf" << endl; //instead of "0" it prints "inf" after sign
    }

    //case when exponent is negative but doesnt equal -127
    else if(exponent < 0)
    {
        //if else statement prints positive or negative symbol when appropriate
        if(sign == 1)
        {
            cout << "-";
        }
        else
            cout << "+";
        
        if(exponent < 0)
        {
            n = (exponent * -1) -1; // when exponent is negative get its absolute value and subtract 1
        }
        else if(exponent > 0)
        {
            n = exponent; //when exponent is postive it can be stored into n as is
        }
        
        cout << "0." << string(n,'0') << "1"; // print "0" "n" amount of times


        //loop prints the significand in binary
        for(int i = 22; i >= 0; --i)
        {
            cout << ((x >> i) & 1);
        }

        cout << endl;
    }

    //case for when exponent equals 0
    else if(exponent == 0)
    {
        //if else statement prints positive or negative symbol when appropriate
       if(sign == 1)
        {
            cout << "-";
        }
        else
            cout << "+";

        cout << "1.";

        //loop prints the significand in binary
        for(int i = 22; i >= 0; --i)
        {
        cout << ((x >> i) & 1);
        }
        cout << endl;
    }

    //case when exponent is positive but not 128
    else if(exponent > 0)
    {
        //if else statement prints positive or negative symbol when appropriate
        if(sign == 1)
        {
            cout << "-";
        }
        else
            cout << "+";

        cout << "1";

        //for loop to print significand in binary
        for(int i = 22; i >= 0; --i)
        {
        cout << ((x >> i) & 1);
        }

        n = exponent - 23; //n is the amount of "0"s to print

        cout << string(n,'0') << ".0" << endl; //print "0" "n" amount of times
    }
}