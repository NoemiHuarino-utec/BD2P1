import csv
from faker import Faker
import random
import unicodedata

# Inicializar Faker con español
fake = Faker('es_ES')

# Opciones predefinidas
ocupaciones = [
    'Ingeniero', 'Medico', 'Abogado', 'Arquitecto', 'Cientifico', 
    'Psicologo', 'Veterinario', 'Analista de datos', 'Programador', 
    'Docente', 'Contador', 'Biotecnologo', 'Enfermero', 'Consultor',
    'Carpintero', 'Plomero', 'Electricista', 'Chef', 'Tecnico de laboratorio', 
    'Marketing Digital', 'Disenador', 'Fotografo', 'Policia', 'Bombero', 
    'Comerciante', 'Futbolista', 'Actor', 'Cantante', 'Agricultor', 'Gerente'
]

estado_civil = ['Soltero/a', 'Casado/a', 'Divorciado/a', 'Viudo/a']
ingresos_mensuales = [(1000, 2000), (2001, 4000), (4001, 7000), (7001, 10000), (10001, 20000)]

# Función para eliminar tildes y reemplazar 'ñ'
def normalizar_texto(texto):
    texto_normalizado = unicodedata.normalize('NFKD', texto).encode('ascii', 'ignore').decode('utf-8')
    return texto_normalizado.replace('ñ', 'n')

# Generar registros sintéticos
def generar_datos(n):
    datos = []
    for _ in range(n):
        dni = fake.unique.random_number(digits=8)  # Generar un DNI único de 8 dígitos
        nombre = normalizar_texto(fake.name())  # Generar un nombre completo y normalizar
        ocupacion = random.choice(ocupaciones)  # Seleccionar una ocupación aleatoria
        estado = normalizar_texto(random.choice(estado_civil))  # Seleccionar y normalizar estado civil
        ingreso = random.randint(*random.choice(ingresos_mensuales))  # Generar ingreso mensual
        edad = random.randint(18, 80)  # Edad entre 18 y 80 años
        sexo = random.choice(['F', 'M'])  # Sexo aleatorio (F o M)
        datos.append([dni, nombre, ocupacion, estado, ingreso, edad, sexo])
    return datos

# Generar registros
registros = generar_datos(20000) # Cantidad de registros a generar

# Crear archivo CSV
nombre_archivo = "AdultDataset.csv"

with open(nombre_archivo, mode='w', newline='', encoding='utf-8') as file:
    writer = csv.writer(file, delimiter=',')  # Usar  coma como separador
    # Escribir encabezado
    writer.writerow(['DNI', 'Nombre', 'Ocupacion', 'Estado Civil', 'Ingreso Mensual', 'Edad', 'Sexo'])
    # Escribir datos
    writer.writerows(registros)

print(f"Nuevo archivo CSV generado: {nombre_archivo}")
