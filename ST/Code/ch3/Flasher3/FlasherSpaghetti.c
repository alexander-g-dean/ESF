// Start Listing Spaghetti_Tasks
void Task_Flash(void) {
	if (g_flash_LED == 1) { 	// Only run task when in flash mode
		Control_RGB_LEDs(1, 1, 1);
	}
	if (g_flash_LED == 1) { 	// Only run task when in flash mode
		Delay(g_w_delay);
	}
	if (g_flash_LED == 1) { 	// Only run task when in flash mode
		Control_RGB_LEDs(0, 0, 0);
	}
	if (g_flash_LED == 1) { 	// Only run task when in flash mode
		Delay(g_w_delay);
	}
}

void Task_RGB(void) {
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		Control_RGB_LEDs(1, 0, 0);
	}
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		Delay(g_RGB_delay);
	}
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		Control_RGB_LEDs(0, 1, 0);
	}
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		Delay(g_RGB_delay);
	}
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		Control_RGB_LEDs(0, 0, 1);
	}
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		Delay(g_RGB_delay);
	}
}
// End Listing Spaghetti_Tasks

// Start Listing Spaghetti_Delay
void Delay(unsigned int time_del, int called_by_Task_Flash) {
	volatile int n;
	while (time_del--) {
		if ( ((called_by_Task_Flash == 1) && (g_Flash_LED == 1)) ||
		((called_by_Task_Flash == 0) && (g_Flash_LED == 0))) {
			n = 1000;
			while (n--)
				;
			}
	}
}
// End Listing Spaghetti_Delay
