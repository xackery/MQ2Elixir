# Elixir

Elixir is a MacroQuest plugin that helps you play EverQuest by playing characters for you.

It is designed to be:
- Simple to install, just `/plugin mq2elixir`, no dependencies required
- Simple to setup, using check marks and slider values in a friendly UI for settings
- Simple to adjust, using your spell gems and hotbars to understand how you play EQ

## Installing Elixir

Elixir currently only works with the latest copy of MacroQuest. If you are using a legacy emu variant, it likely will not load.

- [get MQ2Elixir.dll](https://github.com/xackery/MQ2Elixir/releases), place it inside your macroquest\plugin path.

## Installing and Setting Up MacroQuest

If you don't already own MacroQuest, here is a comprehensive list to get going:

- [get macroquest (Emu Build)](https://github.com/macroquest/macroquest/releases)
- [get MQ2Elixir.dll](https://github.com/xackery/MQ2Elixir/releases), place it inside your macroquest\plugin path
- [get Emu navmesh](https://github.com/Ewiclip/lazarus_navmesh/archive/refs/heads/main.zip), extract it into macroquest\resources\MQ2Nav
- in game, type `/plugin mq2elixir`, you should see elixir loaded
- in your mq dir, there is a subfolder called resources. Create a folder called MQ2Nav inside it, and extract the nav main.zip meshes above into it
- in game, `/plugin mq2nav`
- Right click the MQ system tray icon, and press `Start EQBC server`
- in game, `/plugin mq2dannet`
- in game, `/plugin mq2groupinfo`
- in game, `/plugin mq2autogroup`
- in game, `/autogroup create`
- in game, `/autogroup add dannet`
- in game, `/plugin mq2xptracker`
- /mapfilter npcconcolor show
- /mapfilter npc
- in game, `/nav ui`, go to Settings tab, there'll be a Disabled dropdown - change to Stop Navigation (breaks nav movement)
- press your EQ button, you should see MacroQuest, Settings.
