# SoundMixr
SoundMixr is a simple audio mixer for ASIO devices, but specifically made for SAR (http://sar.audio/), it sees all the channels in the ASIO device and shows them with the option to route any input to any output. You can also split any channel into mono channels, or combine them to make channels with as many lines as you like. Each channel also has a mute, mono, stereo pan and volume parameter. Each channel has its own effects chain in which you can add effects. You have the ability to bypass effects and rearrange them by dragging them to a new position in the chain. You can save/load presets for effects, and even for an entire effect chain. Connect with midi devices and link a Midi CC to any parameter in SoundMixr. Open the Soundboard to load up to 16 files. You can play the files using a midi link, or a hotkey. The Soundboard currently only supports WAV files.

![Image](https://i.ibb.co/7Gr81cP/Screenshot-1302.png)

## Code your own SoundMixr Effects
You can code your own effects as well! There is a Git repo called [EffectBase](https://github.com/KaixoCode/EffectBase) that contains the necessary headers to code a SoundMixr effect. And here is a small example of an effect: [SoundMixrEffect](https://github.com/KaixoCode/SoundMixrEffect). See [Documentation](https://code.kaixo.me/SoundMixr/EffectBase/) for more info.

 If you have any ideas or feedback you can find me on Discord: <code>@Kaixo#0001</code>
