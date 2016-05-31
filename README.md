## 自定义的系统相关API

### io.read (filepath)
### io.write (filepath, data)
### io.readFromZip (filepath)
### io.writeablePath 好像没用
### io.saveInteger (key, value)
### io.getInteger (key, default)
### io.saveString (key, value)
### io.getString (key, default)

### http.post (url, json, callback)
### http.get (url, json, callback)
### http.delete (url, callback)
### http.put (url, callback)

### image.download (url, ifCached, callback)
### image.upload (url, jsonData, callback)

### print (string[])

### setTimeout (callback, mileseconds) return *timer
### clearTimeout (*timer)
### setInterval (callback, millisecondInterval) return *timer
### clearInterval (*timer)