# Architecture

- The raspberry pi runs the [Pipewire](https://pipewire.org/) audio server, which handles low-level audio streaming.
- On top of pipewire, we write applications using [Jack](https://jackaudio.org/), a toolkit that simplifies stream connectivity and processing in C.
- Jack applications consist of a client, that does DSP processing, and a server, that interfaces to the real-time audio devices including DAC and ADC. We will only use the client part of Jack, and use instead the services of pipewire to handle real-time IO.
- To check the current settings used by jack, use the command

``
pw-jack jack_control dp
``

A sample output may look like

```
--- get driver parameters (type:isset:default:value)
             capture: Number of capture ports (uint:notset:2:2)
            playback: Number of playback ports (uint:notset:2:2)
                rate: Sample rate (uint:notset:48000:48000)
             monitor: Provide monitor ports for the output (bool:notset:False:False)
              period: Frames per period (uint:notset:1024:1024)
                wait: Number of usecs to wait between engine processes (uint:notset:21333:21333)
```

## Running an application

## Setting the sample rate

- The Hifiberry card supports the following sample rates: 44100, 48000, 88200, 96000, 176400, 192000
- To set the sample rate, you have to reconfigure the pipewire server
   - change the sample rate in ~/.config/pipewire/pipewire.conf
   - restart the server with ``systemctl --user restart pipewire pipewire-pulse``
 - The commands to change the sample rate are automated in Makefile. The display the current sample rate, use

``
make getrate
``
 - To change the sample rate into one of the supported rates, for example 88200, use

``
make setrate RATE=88200
``
