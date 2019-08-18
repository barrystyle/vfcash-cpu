"""

This package depends on the following external modules:
::
	argparse, csv
::
	
The script can be invoked via:
::
	python -p 

|  @Author: Roland Modnar
|  @Organisation: Modnat Inc. and VFC
|  @Date: 18.08.2019

	* main - usage of this package directly as a script

"""
import argparse
import csv
import requests
import datetime
import os

def isfloat(value):
  try:
    float(value)
    return True
  except ValueError:
    return False

def main():
	""" Main function reading in command line parameters.
	"""
		# construct the argument parse and parse the arguments
	ap = argparse.ArgumentParser()
	ap.add_argument("-k", "--pubkey", default=0, help="public key to send to receive transactions")
	args = vars(ap.parse_args())

	if (args["pubkey"] == 0):
		print("Please enter the public key to receive Transactions")
		quit()

	publicKey = args["pubkey"]

	print("Executing transactions to Public Key: " + publicKey)

	fileName = "minted.priv"
	restAPIUrl = "somesite.com"

	# failedPath = "mintedFailed_" + datetime.now() + ".priv"
	# failedFile = open(failedPath, 'w')


	with open(fileName,'r') as csvfile:
		plots = csv.reader(csvfile, delimiter='/')
		for row in plots:
			privateKey = row[0]
			amount = row[1]
			if (isfloat(amount)):
				fullPost = "http://" + restAPIUrl + "/?" + "from=" + privateKey + "&" + "to=" + publicKey + "&" + "amount=" + amount

				print("Executing transaction of " + amount + " vfc from private Key: " + privateKey + " to public Key: " + publicKey + "with: " + fullPost)
				response = requests.post(fullPost)
				# if response != 201:
					# print("FAILED, printing failed transactions to " + failedPath
					# failedFile.write(privateKey + "\\" + amount)
				# else:
					# print("SUCCESS")
			else:
				print("Transaction \"" + restAPIUrl + "/?" + "from=" + privateKey + "&" + "to=" + publicKey + "&" + "amount=" + amount + "\" has a weird format" )

	backFilePath = "minted-archived_" + str(datetime.date.today()) + ".priv"
	os.rename(fileName, backFilePath)
	mintedFile = open(fileName, 'w')
	mintedFile.close()

if __name__== "__main__":
  main()