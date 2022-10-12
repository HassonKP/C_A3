/*Name: Killian Hasson		Date: 02/16/2022		Class:CS201
 *
 * For this program we were tasked with writing a program in C that would take in a
 *number of fraction bits, exponenet bits, and a hexadicmal value. The program needed
 *to then act as an IEEE floating point parser for said input bit values. The program 
 *was extremely difficult and I had a really hard time getting my head around the arbitrary
 *number of bit settings, 
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//Function for extracting sign, exponent, and fraction bits using masking
int left_mask(unsigned int * result, unsigned int bits, unsigned int value)
{
  	unsigned int mask = 0;
	//Loop through the bits
  	for (int i = 0; i < bits; i++) 
  	{
    		//Shift the mask left one place
    		mask = mask << 1;
    		//'or' on another 1, filling the mask with 1's
    		mask = mask | 1;
  	}
	//Write in the results using the parameter passed by reference
  	*result = value & mask;
  	return 1;
}

//Function for dealing with fraction binary
float binary_fraction(unsigned int bits, unsigned int value) 
{
  	//Formula calls for Bit * 2^n + Bit * 2^n-1 + ...
  	float retval = 0.0;
	//First exponenet value of bit
  	int exp = -1;
	//Loop through the bits
	for (int i = bits - 1; i >= 0; i--) 
	{
		if (value & (1 << i)) 
		{
			//Carry out formula
      			retval += pow(2, exp);
    		}
		//decrease exponenet
    		exp -= 1;
  	}
	//return the value
  	return retval;
}

int main(int argc, char * argv[])
{
	//u-ints passed in for respective fields
	unsigned int frac_bits;
	unsigned int exp_bits;
	unsigned int hex_value;
  	//u-ints for getting respective bits from masking
	unsigned int sign_bit;
	unsigned int exponent;
	unsigned int fraction;
	//u-int for holding the exponent bias
	unsigned int bias;
  	//floats for holding values post function calls
	float fraction_converted;
	float result;
	//E and M used for formulas
	int E;
	float M;
	//pointer for arguments passed in
	char * next;

	//Basic conditional check for number of arguments passed in
	if (argc != 4) 
	{
		printf("Usage: fp_parse <# of frac_bits> <# of exp_bits>\n");
		return 1;
	}

	//Assign input value to frac_bits, check it fits in required input
  	frac_bits = strtoul(argv[1], &next, 10);
  	if ((int)*next != 0 || frac_bits < 2 || frac_bits > 10) 
	{
		//Display error
		printf("Invalid number of fraction bits (%u). Should be between 2 and 10.\n", frac_bits);
		return 1;
	}

	//Assign input value to exp_bits, check it fits required input
	exp_bits = strtoul(argv[2], &next, 10);
	if ((int)*next != 0 || exp_bits < 3 || exp_bits > 5) 
	{
		//Display error
		printf("Invalid number of exponent bits (%u). Should be between 3 and 5.\n", exp_bits);
		return 1;
	}

	//Assign input to hex_value
	hex_value = strtoul(argv[3], &next, 16);
	//Check if argument is a hex number
	if ((int)*next != 0) 
	{
		// error
		printf("Argument 3 is not a hexadecimal number (%s).\n", argv[3]);
		return 1;
	}
	//Check if the hex value fits max number of bits
	if(hex_value > (1 << (exp_bits + frac_bits + 1)) - 1)
	{
		printf("Number %s exceeds maximum number of bits. Only %d allowed here.\n", argv[3], exp_bits + frac_bits + 1);
		return 1;
	}

	//Use left_mask function to return the correct subset of bits for the fraction bits, exponenet bits, and sign bit
	left_mask(&fraction, frac_bits, hex_value);
  	left_mask(&exponent, exp_bits, hex_value >> frac_bits);
  	left_mask(&sign_bit, 1, hex_value >> (frac_bits + exp_bits));
	//Call the finary fraction function passing frac_bits and hex_value
  	fraction_converted = binary_fraction(frac_bits, hex_value);
  	//Use exponent bias formula
	bias = pow(2, exp_bits - 1) - 1;

  	// zero?
  	if (exponent == 0 && fraction == 0) 
	{
    		// value is zero
    		if (sign_bit)
      		printf("-");
    		printf("%f\n", 0.0);
    		return 0;
  	}
  
	//inf, -inf, NaN display?
  	// shift 1 right by 1-more-than exp_bits,then subtract 1 to make a value that is all 1's for exp_bits bits
  	if (exponent == ((1 << exp_bits) - 1)) 
	{
    		//Exponent is all ones, dealing with inf, -inf, or NaN
    		if (fraction) 
		{
      			printf("NaN\n");
      			return 0;
    		}
    		if (sign_bit)
      			printf("-");
    		else
      			printf("+");
    		
		printf("inf\n");
    		return 0;
  	}

  	//Calculate E and M for final formula
  	E = exponent - bias;
 	M = 1.0 + fraction_converted;
  	if (exponent == 0) 
	{
    		E = 1 - bias;
    		M = fraction_converted;
  	}

	//Final formula application
	result = pow(-1, sign_bit) * M * pow(2, E);
  	printf("%f\n", result);

	return 0;

}
