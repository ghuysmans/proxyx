for k, v in pairs(http['headers']) do
	print(k..": "..v)
end
http['say']("native printf")
return t[0]+t[1]
