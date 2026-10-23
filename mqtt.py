import paho.mqtt.client as mqtt


BROKER = "broker.hivemq.com"

PORTA = 1883


TOPICO_COMANDO = "senai510/microondas/comando"

TOPICO_TEMPO = "senai510/microondas/tempo"

TOPICO_STATUS = "senai510/microondas/status"


tempo_atual = 0

status_atual = "PARADO"


def on_connect(client, userdata, flags, reason_code, properties):

    print("Conectado ao HiveMQ!")

    client.subscribe(TOPICO_TEMPO)

    client.subscribe(TOPICO_STATUS)


def on_message(client, userdata, msg):

    global tempo_atual
    global status_atual


    mensagem = msg.payload.decode()


    print("Mensagem recebida!")

    print("Topico:", msg.topic)

    print("Mensagem:", mensagem)


    if msg.topic == TOPICO_TEMPO:

        tempo_atual = int(mensagem)


    elif msg.topic == TOPICO_STATUS:

        status_atual = mensagem


client = mqtt.Client(
    mqtt.CallbackAPIVersion.VERSION2,
    client_id="flask-microondas"
)


client.on_connect = on_connect

client.on_message = on_message


def iniciar_mqtt():

    client.connect(BROKER, PORTA)

    client.loop_start()


def enviar_tempo(tempo):

    client.publish(
        TOPICO_COMANDO,
        str(tempo)
    )