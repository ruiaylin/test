
//	DES function prototypes

#ifdef __cplusplus
extern "C" {
#endif

int desinit(int mode);
void desdone(void);
void des_setkey(char *subkey, char *key);
void endes(char *block, char *subkey);
void dedes(char *block, char *subkey);

#ifdef __cplusplus
}
#endif
