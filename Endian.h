#ifndef ENDIAN_H_
#define ENDIAN_H_

// Endianness conversion inlines - These will be optimised out on platforms where
//  it's not necessary, and will collapse to only the swapping code on 
//  other big endian platforms.

inline bool littleEndian()
{
	uint16_t tn = 0xff00;
	if(((uint8_t*)&tn)[0] == 0x00)
		return true;
	else 
		return false;
}

inline uint16_t NativeToLittle(uint16_t in)
{
	if(littleEndian())
		return in;
	else
	{
		uint16_t var = 0;
		((uint8_t*)&var)[0] = ((uint8_t*)&in)[1];
		((uint8_t*)&var)[1] = ((uint8_t*)&in)[0];

		return var;
	}
}

inline uint32_t NativeToLittle(uint32_t in)
{
	if(littleEndian())
		return in;
	else
	{
		uint32_t var = 0;
		((uint16_t*)&var)[0] = NativeToLittle(((uint16_t*)&in)[1]);
		((uint16_t*)&var)[1] = NativeToLittle(((uint16_t*)&in)[0]);

		return var;	
	}
}

inline uint64_t NativeToLittle(uint64_t in)
{
	if(littleEndian())
		return in;
	else
	{
		uint64_t var = 0;
		((uint32_t*)&var)[0] = NativeToLittle(((uint32_t*)&in)[1]);
		((uint32_t*)&var)[1] = NativeToLittle(((uint32_t*)&in)[0]);

		return var;
	
	}
}

inline uint64_t LittleToNative(uint64_t in)
{
	if(littleEndian())
		return in;
	else
	{
		uint64_t var = 0;
		((uint32_t*)&var)[0] = NativeToLittle(((uint32_t*)&in)[1]);
		((uint32_t*)&var)[1] = NativeToLittle(((uint32_t*)&in)[0]);

		return var;
	
	}
}


#endif //ENDIAN_H_