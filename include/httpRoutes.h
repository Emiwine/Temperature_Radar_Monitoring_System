#ifndef HTTP_ROUTES_H
#define HTTP_ROUTES_H

void initializeRoutes();
// void handleConfiguration();

void handleNotFound();
void handleRestartDevice();
void getStatus();
void handleHttpClient();
void handleConfiguration();
void handleconfigMFID();
void handleconfigDeviceID();

#endif
