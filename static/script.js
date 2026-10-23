
function digitarNumero(numero) {
    const campo = document.getElementById("segundos");

    let valor = campo.value;

    if (valor === "00" || valor === "0") {
        valor = "";
    }

    if (valor.length < 2) {
        valor += numero;
    }

    if (Number(valor) <= 99) {
        campo.value = valor;
        document.getElementById("tempo").textContent =
            valor.padStart(2, "0");
    }
}

function limparTempo() {
    document.getElementById("segundos").value = "00";
    document.getElementById("tempo").textContent = "00";
}

function apagarNumero() {
    const campo = document.getElementById("segundos");

    let valor = campo.value.slice(0, -1);

    if (valor === "") {
        valor = "00";
    }

    campo.value = valor;

    document.getElementById("tempo").textContent =
        valor.padStart(2, "0");
}

async function iniciarMicroondas() {
    const campo = document.getElementById("segundos");
    const status = document.getElementById("status");

    const tempo = Number(campo.value);

    if (!Number.isInteger(tempo) || tempo < 1 || tempo > 99) {
        alert("Escolha um tempo entre 1 e 99 segundos!");
        return;
    }

    status.textContent = "ENVIANDO...";

    try {
        const resposta = await fetch("/iniciar", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                tempo: tempo
            })
        });

        const dados = await resposta.json();

        if (!resposta.ok) {
            status.textContent = "ERRO";
            alert(dados.erro || "Não foi possível iniciar.");
            return;
        }

        status.textContent = "COMANDO ENVIADO";

        console.log("Tempo enviado:", dados.tempo);

    } catch (erro) {
        status.textContent = "ERRO DE CONEXÃO";
        alert("Não foi possível comunicar com o Flask.");
        console.error(erro);
    }
}

// Busca o tempo e o status recebidos pelo Flask
async function atualizarStatus() {
    try {
        const resposta = await fetch("/status");
        const dados = await resposta.json();

        if (dados.tempo !== null && dados.tempo !== undefined) {
            document.getElementById("tempo").textContent =
                String(dados.tempo).padStart(2, "0");
        }

        if (dados.status) {
            document.getElementById("status").textContent =
                dados.status;
        }

    } catch (erro) {
        console.error("Erro ao consultar status:", erro);
    }
}

// Atualiza o visor a cada meio segundo
atualizarStatus();
setInterval(atualizarStatus, 500);