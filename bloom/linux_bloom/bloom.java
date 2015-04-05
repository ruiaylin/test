package bloom;

public class bloom
{
	/* A Simple Hash Function */
	private int simple_hash(String str)
	{
		int idx;
		int hash;
		int len = str.length();

		for (hash = 0, idx = 0; idx < len; idx++) {
			hash = 31 * hash + str.charAt(idx);
		}

		return (hash & 0x7FFFFFFF);
	}

	/* RS Hash Function */
	private int RS_hash(String str)
	{
		int b = 378551;
		int a = 63689;
		int hash = 0;
		int idx  = 0;
		int len  = str.length();

		for (idx = 0; idx < len; idx++) {
			hash = hash * a + str.charAt(idx);
			a *= b;
		}

		return (hash & 0x7FFFFFFF);
	}

	private int JS_hash(String str)
	{
		int idx  = 0;
		int hash = 1315423911;
		int len  = str.length();

		for (idx = 0; idx < len; idx++) {
			hash ^= ((hash << 5) + str.charAt(idx) + (hash >> 2));
		}

		return (hash & 0x7FFFFFFF);
	}

	/* P. J. Weinberger Hash Function */
	private int PJW_hash(String str)
	{
		int BitsInUnignedInt	= 32;
		int ThreeQuarters		= ((BitsInUnignedInt * 3) / 4);
		int OneEighth			= (BitsInUnignedInt / 8);

		int hash		= 0;
		int test		= 0;
		int idx			= 0;
		int HighBits	= (0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
		int len			= str.length();

		for (idx = 0; idx < len; idx++) {
			hash = (hash << OneEighth) + str.charAt(idx);
			if ((test = hash & HighBits) != 0)	{
				hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
			}
		}

		return (hash & 0x7FFFFFFF);
	}

	/* ELF Hash Function */
	private int ELF_hash(String str)
	{
		int hash = 0;
		int x    = 0;
		int idx  = 0;
		int len = str.length();

		for (idx = 0; idx < len; idx++) {
			hash = (hash << 4) + str.charAt(idx);
			if ((x = hash & 0xF0000000L) != 0)	{
				hash ^= (x >> 24);
				hash &= ~x;
			}
		}

		return (hash & 0x7FFFFFFF);
	}

	/* BKDR Hash Function */
	private int BKDR_hash(String str)
	{
		int seed = 131; // 31 131 1313 13131 131313 etc..
		int hash = 0;
		int idx  = 0;
		int len = str.length();

		for (idx = 0; idx < len; idx++) {
			hash = hash * seed + str.charAt(idx);
		}

		return (hash & 0x7FFFFFFF);
	}

	/* SDBM Hash Function */
	private int SDBM_hash(String str)
	{
		int idx = 0;
		int hash = 0;
		int len = str.length();

		for (idx = 0; idx < len; idx++) {
			hash = str.charAt(idx) + (hash << 6) + (hash << 16) - hash;
		}

		return (hash & 0x7FFFFFFF);
	}

	/* DJB Hash Function */
	private int DJB_hash(String str)
	{
		int idx  = 0;
		int hash = 5381;
		int len = str.length();

		for (idx = 0; idx < len; idx++) {
			hash += (hash << 5) + (*str.charAt(idx);
		}

		return (hash & 0x7FFFFFFF);
	}

	/* AP Hash Function */
	private int AP_hash(String str)
	{
		int idx = 0;
		int hash = 0;
		int len = str.length();

		for (idx = 0; idx < len; idx++) {
			if ((idx & 0x1) == 0) {
				hash ^= ((hash << 7) ^ str.charAt(idx) ^ (hash >> 3));
			} else {
				hash ^= (~((hash << 11) ^ str.charAt(idx) ^ (hash >> 5)));
			}
		}

		return (hash & 0x7FFFFFFF);
	}

	/* CRC Hash Function */
	private int CRC_hash(String str)
	{
		int            nleft   = strlen(str);
		long  sum     = 0;
		short int *w       = (unsigned short int *)str;
		int short int  answer  = 0;

		/*
		* Our algorithm is simple, using a 32 bit accumulator (sum), we add
		* sequential 16 bit words to it, and at the end, fold back all the
		* carry bits from the top 16 bits into the lower 16 bits.
		*/
		while ( nleft > 1 ) {
			sum += *w++;
			nleft -= 2;
		}
		/*
		* mop up an odd byte, if necessary
		*/
		if ( 1 == nleft ) {
			*( unsigned char * )( &answer ) = *( unsigned char * )w ;
			sum += answer;
		}
		/*
		* add back carry outs from top 16 bits to low 16 bits
		* add hi 16 to low 16
		*/
		sum = ( sum >> 16 ) + ( sum & 0xFFFF );
		/* add carry */
		sum += ( sum >> 16 );
		/* truncate to 16 bits */
		answer = ~sum;

		return (answer & 0xFFFFFFFF);
	}

	public static int main(String[] args)
	{
		System.out.println("hello");
	}
}

