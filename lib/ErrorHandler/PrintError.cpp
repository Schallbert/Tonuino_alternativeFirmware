#if 0
// TODO: implement


// onStartup
#if DEBUGSERIAL
    Arduino_interface_com *pSerial = m_pArduinoHal->getSerial();
    pSerial->com_begin(DEBUGSERIAL_BAUDRATE); // Some debug output via serial
    pSerial->com_println("Booted, now initializing");
#endif

#if DEBUGSERIAL
    pSerial->com_println("Complete.");
#endif

// onShutdown
#if DEBUGSERIAL
    m_pArduinoHal->getSerial()->com_println("Shutting down...");
#endif

// loop
#if DEBUGSERIAL
    m_pMp3Ctrl->print_debug_message();
    m_pNfcControl->print_debug_message();
    //systemCtrl->print_debug_message(); // TODO: IMPLEMENT
#endif

#endif