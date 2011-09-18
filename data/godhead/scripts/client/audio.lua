Sound.music_mode = "menu"

--- Switches the music track.
-- @param clss Sound class.
-- @param mode Music mode. ("boss"/"char"/"game"/"menu")
Sound.switch_music_track = function(clss, mode)
	local modes = {
		boss = {"Spiderbite_Lucidity", "Pits"},
		char = {"Arena", "Bad_Foresense"},
		game = {"Solitude", "Shadows", "Stressed", "Townside_Gloom",
		        "Clouded", "Feudal", "Marketeer"},
		intro = {"Poet's_Lamentation"},
		menu = {"Poet's_Lamentation"}};
	if mode then clss.music_mode = mode end
	local tracks = modes[clss.music_mode];
	clss.music_fading = 1.5
	clss.music_looping = (#tracks > 1)
	clss.music_volume = Client.views.options.music_volume
	clss.music = tracks[math.random(1, #tracks)]
end
