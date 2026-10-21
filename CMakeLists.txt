COMPONENTS_CONFIGURE_COMPONENT(MCU "1.0.0")
if (COMPONENTS_IS_ENABLED_QT)
    target_link_libraries(Components-MCU PRIVATE Qt5::)
endif()
