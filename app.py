from flask import Flask, render_template, request, jsonify

import mqtt


app = Flask(__name__)


@app.route("/")
def index():

    return render_template("index.html")


@app.route("/iniciar", methods=["POST"])
def iniciar():

    dados = request.json

    tempo = int(dados["tempo"])


    if tempo < 1 or tempo > 99:

        return jsonify({
            "erro": "Escolha um tempo entre 1 e 99 segundos."
        }), 400


    mqtt.enviar_tempo(tempo)


    return jsonify({
        "mensagem": "Micro-ondas iniciado!",
        "tempo": tempo
    })


@app.route("/status")
def status():

    return jsonify({

        "tempo": mqtt.tempo_atual,

        "status": mqtt.status_atual

    })


if __name__ == "__main__":

    mqtt.iniciar_mqtt()

    app.run(debug=True)