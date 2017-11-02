import urllib2
import subprocess

def find_between( s, first, last ):
    try:
        start = s.index( first ) + len( first )
        end = s.index( last, start )
        return s[start:end]
    except ValueError:
        return ""

def run():
    root_url = raw_input("Please input url: ")
    depth = raw_input("Please input depth level: ")
    print subprocess.check_call(['./a.out', root_url, depth])
    id = 595 #id of URL

    #read in URLs and corresponding feature vectors of 40 training instances from a .txt. file, using amazon.com as an example for now

    hostnames = ["amazon.com"]
    for hostname in hostnames:
        some_url = "http://api.urlvoid.com/api1000/af774a8d258f8c97e0595f90f62cf5b75d5e5b32/host/" + hostname + "/"
        content = urllib2.urlopen(some_url).read()
        count = content.find("<count>")
        if (count): #URL not benign
            classification = find_between(content, "<count>", "</count>")
            #URLVoid provides a classification from 0 to 34 out of 34, with 0 out of 34 equivalent to -1, or a benign
            #classification, and 34 out of 34 equivalent to 1, or a malignant classification. If the classification is
            #0 out of 34, then count is false, and we skip to the else statement. The code below alters the URLVoid
            #classification to fit our purposes.
            if (classification == 16.5):
                classification = 0
            elif (classification == 34):
                classification = 1
            elif (classification > 0 && classification < 16.5):
                classification = - ((34 - classification) / 34)
            elif (classification > 16.5 && classification < 34):
                classification = (classification / 34)

            #append the following content to our_dataset.txt: id, classification, feature vector

        else: #URL benign
            #append the id, classification = -1, feature vector to our_dataset.txt

        id++


    #run a.out again to determine new accuracy (hopefully better!)


























if __name__ == '__main__':
    run()
