/* DOS bios.h shim */
#define _KEYBRD_READ 0
#define _KEYBRD_READY 1
#define _KEYBRD_SHIFTSTATUS 2
static unsigned _bios_keybrd(unsigned cmd){
    (void)cmd; return 0; }
