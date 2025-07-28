# Architecture

- The raspberry pi runs the [Pipewire](https://pipewire.org/) audio server, which handles low-level audio streaming.
- On top of pipewire, we write applications using [Jack](https://jackaudio.org/), a toolkit that simplifies stream connectivity and processing in C.
- Jack applications consist of a client, that does DSP processing, and a server, that interfaces to the real-time audio devices including DAC and ADC. We will only use the client part of Jack, and use instead the services of pipewire to handle real-time IO.
