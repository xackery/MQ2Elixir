# MQ2Elixir

3 Modes of interactiveness
1) click passive abilities like spurn on SK's
2) dire mode where if player or group is detected to be in a dire situation, help (click instant cast saves, fades, etc)
3) normal mode where all normal activity is attempted


Rules of engagement:

if invis, don't break it. 
if nav is active, only use instant spells

TLOs:

Note that spell name does NOT need Rk. suffix, and buffs do not need roman numeral suffix

* ${Elixir.Stance} returns current stance as an int
* ${Elixir.IsFacingTarget} returns true if you are facing your target
* ${Elixir.TargetHasBuff[spell name]} likely not needed, testing, checks if target has buff
* ${Elixir.SpawnIDHasBuff[123 spell name]} does provided spawn Id have a buff? uses an internal cache system
* ${Elixir.Memorize[12 spell name] returns true if invocation to memorize spell name to gem 12 succeeds
* ${Elixir.Cast[spell name]} returns true if spell name successfully casted via all possible ways, this tries in order: spell, ability, item, aa, combat ability
* ${Elixir.CastItem[spell name]} returns true if item cast invocation succeeds
* ${Elixir.CastSpell[spell name]} returns true if spell cast invocation succeeds
* ${Elixir.CastAbility[spell name]} returns true if ability invocation succeeds
* ${Elixir.CastCombatAbility[spell name]} returns true if ability succeeds
* ${Elixir.CastAA[spell name]} returns true if aa succeeds


## Setup
- Clone https://gitlab.com/redguides/VeryVanilla
- Inside root, run MQ2Auth.exe, and hit Save
- run VanillaDIY.sln opening it in Visual Studio 2019
- Build menu -> Build Solution
- Right click solution on top right area, Add -> Existing Project, select MQ2Elixir.vcproj
- Right click newly added project MQ2Elixir, Properties, change from Win XP 2013 to Visual Studio 2019 x142
- Build menu -> Build Solution again