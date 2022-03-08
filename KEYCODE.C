/*

Output the scancode (keycode) for each key pressed

*/

#include <stdlib.h>
#include <stdio.h>

#include "allegro.h"


int main()
{
   int k;

   allegro_init();
   install_keyboard(); 

   /* keyboard input can be accessed with the readkey() function */
   printf("\nPress some keys (ESC to finish)\n");
   do {
      k = readkey();
      printf("keycode is %-3d %-3d\n", k>>8,k&255);
   } while ((k & 0xFF) != 27);

   return 0;
}
