import numpy as np
import pandas as pd

# Columns in the CSV files
col_list = ["sent.seconds", "sent.microseconds", "received.seconds", "received.microseconds","sampleTimeStamp.seconds","sampleTimeStamp.microseconds","groundSteering"]

# Read the CSV files as pandas dataframes
rec1 = pd.read_csv('rec3.csv', usecols=col_list, sep=';')
rec2 = pd.read_csv('lightning.csv', usecols=col_list, sep=';')

accepted = 0
frames = 0

orgRec = ''

# For loop that iterates through the rows in both csv files
for (idxRow, s1), (_, s2) in zip(rec1.iterrows(), rec2.iterrows()):
    # Variable that increases by one once for every frame, 
    # will in the end give us the total amount of frames
    frames = frames + 1

    # For loop that iterates through the columns in the rows
    for (idxCol, GSR1), (_, GSR2) in zip(s1.iteritems(), s2.iteritems()):
        # Focus on groundSteering column
        orgRec = orgRec + str(GSR2) + ', '
        if(idxCol == 'groundSteering'):
            # If the GSR is 0, upper and lower bounds should be 0.05
            if GSR1 == 0:
                upper = 0.05
                lower = -0.05
            # If GSR is larger than 0, set upper and lower bounds
            # Upper and lower bounds should be different,
            # depending on if the GSR is positive or negative
            elif GSR1 > 0:
                upper = GSR1 * 1.5
                lower = GSR1 * 0.5
            # If GSR is less than 0, set upper and lower bounds
            else:
                upper = GSR1 * 0.5
                lower = GSR1 * 1.5
            # If GSR is within the bounds, the frame is accepted
            if(GSR2 <= upper and GSR2 >= lower):
                accepted = accepted + 1

# Calculate percentage of accepted frames
percentage = (accepted / frames) * 100

print('Percentage: ', percentage, '%')
print('Accepted frames: ', accepted)
print(orgRec)