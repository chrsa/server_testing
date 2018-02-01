#!/usr/bin/env python3

# from specification ftp://ftp.bmf.com.br/MarketData/OFER_CPA_LAYOUT_english.txt
# details
instr_pos = 12 	# instrument position
instr_len = 50 	# instrument length 
# header
nl_pos 	= 46 	# number of lines (position)
nl_len 	= 9	# number of lines (length)

# dictionary
counter = {}

# the most liquid instrument
most_liquid = ""
counter[most_liquid] = -1;


for market_data in ["OFER_CPA_BMF_20180126.TXT", "OFER_VDA_BMF_20180126.TXT"]:
	# opening file
	f = open(market_data, "r")

	# read the header
	header = f.readline()
	number_lines = int(header[nl_pos -1: nl_pos + nl_len -1])
	number_lines -= 2 # removing header and trailer of the counting

	# for each line from the file (except header) 
	for line in f:
		if number_lines == 0 :
			break
		else:
			number_lines -= 1;

		instrument = line[instr_pos -1:instr_pos + instr_len -1]
		if instrument in counter :
			counter[instrument] += 1;
		else :
			counter[instrument] = 1;		

		if counter[instrument] > counter[most_liquid] :
			most_liquid = instrument
		
	
	#closing file
	f.close();

		
print(str(most_liquid))


