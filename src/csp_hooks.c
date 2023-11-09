void csp_shutdown_hook(void) {
    printf("Shutting Down!!\n");
    system("shutdown now");
}

void csp_reboot_hook(void) {
    printf("Rebooting!!\n");
    system("reboot");
}
