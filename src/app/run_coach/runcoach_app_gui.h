#ifndef _RUNCOACH_APP_GUI_H
    #define _RUNCOACH_APP_GUI_H
    #include <stdint.h>
    
    void runcoach_app_gui_setup( uint32_t tile_num );
    void runcoach_update_labels();
    void runcoach_update_sections_label();
    void runcoach_set_run_time_open_btn_status(bool enabled);
    void runcoach_show_runtime_screen();
    void runcoach_show_main_screen();
#endif // _RUNCOACH_APP_GUI_H