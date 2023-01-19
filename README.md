# new_CChat

Chat cifrata in terminale usando protocollo TLS con la libreria [LibreSSL](https://github.com/libressl/portable)

Per eseguire il server bisogna specificare come parametro la porta di ingresso (es.: <code>./CChat 6767</code>)<br>
Per eseguire il client bisogna specificare come parametro l'indirizzo ip del server e la sua porta di ingresso
(es.: <code>./CChat 172.22.248.32 6767</code>)<br>
è possibile ottenere l'indirizzo IP del server con il comando <code>ifconfig</code>
