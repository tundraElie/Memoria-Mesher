import subprocess

DIR = "/home/taiga/Desktop/U/mesher/"

## compilar
cmake = subprocess.Popen(["cmake","../src/"], cwd=DIR+"build").communicate()[0]
make = subprocess.Popen(["make"],
                        cwd=DIR+"build",
                        shell=True,
                        stdout=subprocess.PIPE,
                        universal_newlines=True).communicate()[0]
print(make)
make.strip()
if make[(len(make)-31):-1] == "[100%] Built target mesher_roi":
  mesher_roi = subprocess.Popen(["../build/mesher_roi"],
                                cwd=DIR+"data").communicate()[0]
  # print(mesher_roi)
  ## Creacion de Archivo
  test_file = subprocess.Popen(["../build/mesher_roi","-d","guide.mdl","-s", "4", "-u", "gs4", "-m", "-v"],
                              cwd=DIR+"data").communicate()[0]

  ## Vizualizar Objeto
  process = subprocess.Popen(["../../view/build/viewer","-m","gs4.m3d"], cwd=DIR+"data",bufsize=0,
                             stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)

  process.stdin.write(('%s\n' % "n").encode())

  output = process.communicate()[0]
  print(output)
else:
  print("Error al compilar")

