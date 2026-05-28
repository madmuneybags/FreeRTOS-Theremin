# FreeRTOS-Theremin

This project consists of a 'digital' theremin instrument built using an STM32, two US-100 Ultrasonic Distance sensors, and a speaker.

While a true theremin changes its pitch and volume using the human body’s natural electrical field, this device used the distance reported by the distance sensors to adjust pitch and volume.

This device utilizes FreeRTOS to monitor two proximity sensors constantly to adjust pitch and volume. An DAC is used to produce a sine wave with frequency based on the pitch to power the frequency. The system utilizes a number of tasks and queues to ensure data integrity 

