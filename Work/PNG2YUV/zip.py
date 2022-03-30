import zlib
list_dec = []
f = open('C:\\Users\\sdlwq\\Desktop\\Github\\Data-Compression\\Work\\PNG2YUV\\out.idat', 'rb')
data = f.read()
new_data = zlib.decompress(data)
fp = open('in.idat','wb')
fp.write(new_data)
fp.close()
print('done')