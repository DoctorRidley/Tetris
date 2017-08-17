
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

#ifndef BIT_H
#define BIT_H

////////////////////////////////////////////////////////////////////////////////
//Functionality - Sets bit on a PORTx
//Parameter: Takes in a uChar for a PORTx, the pin number and the binary value 
//Returns: The new value of the PORTx
unsigned short SetBit(unsigned short pin, unsigned char number, unsigned char bin_value) 
{
	return (bin_value ? pin | (0x0001 << number) : pin & ~(0x0001 << number));
}

////////////////////////////////////////////////////////////////////////////////
//Functionality - Gets bit from a PINx
//Parameter: Takes in a uChar for a PINx and the pin number
//Returns: The value of the PINx
unsigned short GetBit(unsigned short port, unsigned char number) 
{
	return ( port & (0x0001 << number) );
}

#endif //BIT_H