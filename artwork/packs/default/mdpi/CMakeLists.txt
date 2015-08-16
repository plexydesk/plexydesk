set(icon_dpi "mdpi")

file(GLOB action_icons actions/*.png)
file(GLOB navigation_icons navigation/*.png)
file(GLOB panel_icons panel/*.png)


install(FILES ${action_icons} DESTINATION plexydesk.app/Contents/Resources/packs/default/${icon_dpi}/actions/)
install(FILES ${navigation_icons} DESTINATION plexydesk.app/Contents/Resources/packs/default/${icon_dpi}/navigation/)
install(FILES ${panel_icons} DESTINATION plexydesk.app/Contents/Resources/packs/default/${icon_dpi}/panel/)
