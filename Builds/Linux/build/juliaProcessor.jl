function myprocess(data)
	if data>0
		return 200*abs(cos(data/100))
	else
		return 200*(cos(data))
	end
end
