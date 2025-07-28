# Architecture

- The raspberry pi runs the [Pipewire](https://pipewire.org/) audio server, which handles low-level audio streaming.
- On top of pipewire, we write applications using [Jack](https://jackaudio.org/), a toolkit that simplifies stream connectivity and processing in C.
- Jack applications consist of a client, that does DSP processing, and a server, that interfaces to the real-time audio devices including DAC and ADC. We will only use the client part of Jack, and use instead the services of pipewire to handle real-time IO.
- To check the current settings used by jack, use the command

```
pw-jack jack_control dp
```

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

- Compiling and running an application under jack involves the following steps:
     - Compile a C program using the jack API into a jack compatible client. The client defines a number of audio input ports and a number of output output ports. The ports carry audio streams that are transferred to and from the C application as frames of N samples.
     - Run the application run ``pw-jack application_name``
     - Connect the audio ports defined in the jack application to pipewire interfaces. An easy to use application that supports this operation is qpwgraph.
- To compile the application, select one of the subdirectories and compile. For example:

```
pschaumont@rpi4d01:~/jackdsp $ cd loop-stereo/
pschaumont@rpi4d01:~/jackdsp/loop-stereo $ ls
Makefile  simple-client.c
pschaumont@rpi4d01:~/jackdsp/loop-stereo $ make
gcc simple-client.c -o simple-client -ljack
```

- To run the application, execute it using the pipewire jack interface

```
pschaumont@rpi4d01:~/jackdsp/loop-stereo $ pw-jack ./simple-client
engine sample rate: 48000
```
- Connect the jack ports to pipewire HifiBerry ports with ```qpwgraph```
- After connecting the ports, check the running program with ```pw-top```. The output should show what audio streams are active

```
S   ID  QUANT   RATE    WAIT    BUSY   W/Q   B/Q  ERR FORMAT           NAME
I   30      0      0   0.0us   0.0us  ???   ???     0                  Dummy-Driver
S   31      0      0    ---     ---   ---   ---     0                  Freewheel-Driver
S   39      0      0    ---     ---   ---   ---     0                  Midi-Bridge
S   60      0      0    ---     ---   ---   ---     0                  v4l2_input.platform-bcm2835-isp.2
S   62      0      0    ---     ---   ---   ---     0                  v4l2_input.platform-bcm2835-isp.3
S   64      0      0    ---     ---   ---   ---     0                  v4l2_input.platform-bcm2835-isp.6
S   66      0      0    ---     ---   ---   ---     0                  v4l2_input.platform-bcm2835-isp.7
S   72      0      0    ---     ---   ---   ---     0                  alsa_output.platform-bcm2835_audio.ste
R   71   1024  48000 172.2us   1.4us  0.01  0.00    0    S32LE 2 48000 alsa_input.platform-soc_sound.stereo-f
R   34      0      0  25.4us 143.9us  0.00  0.01    0    S32LE 2 48000  + alsa_output.platform-soc_sound.ster
R   81      0      0  11.1us   2.4us  0.00  0.00    0                   + simple
```

- An alternate, text-based method to connect the jack ports is as follows. First, find the list of available input ports and output ports

```
pw-link -i
```

and

```
pw-link -o
```

- Next, connect the ports. Output ports connect to input ports. For example, the following connects the output of the jack application to the HifiBerry

```
pw-link simple:output_left alsa_output.platform-soc_sound.stereo-fallback:playback_FL
pw-link simple:output_right alsa_output.platform-soc_sound.stereo-fallback:playback_FR
```

The following connects the output of the Hifiberry to the input of the jack application

```
pschaumont@rpi4d01:~ $ pw-link alsa_input.platform-soc_sound.stereo-fallback:capture_FL simple:input_left
pschaumont@rpi4d01:~ $ pw-link alsa_input.platform-soc_sound.stereo-fallback:capture_FR simple:input_right
```

## Setting the sample rate

- The Hifiberry card supports the following sample rates: 44100, 48000, 88200, 96000, 176400, 192000
- To set the sample rate, you have to reconfigure the pipewire server
   - change the sample rate in ~/.config/pipewire/pipewire.conf
   - restart the server with ``systemctl --user restart pipewire pipewire-pulse``
 - The commands to change the sample rate are automated in Makefile. The display the current sample rate, use

```
make getrate
```
 - To change the sample rate into one of the supported rates, for example 88200, use

```
make setrate RATE=88200
```
