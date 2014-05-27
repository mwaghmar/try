import java.security.spec.*;
import java.security.KeyFactory.*;
import java.security.spec.*;
import java.security.*;
import sun.misc.*;
import java.io.*;
public class checkSignature 
{
    public static boolean check(String clientPublicKey, String request,String response) {
  try {
     KeyFactory kf = KeyFactory.getInstance("RSA");
     BASE64Decoder bd = new BASE64Decoder();
     X509EncodedKeySpec pk = new X509EncodedKeySpec(bd.decodeBuffer(clientPublicKey));
     PublicKey generatedPublicKey = kf.generatePublic(pk);
      
   byte[] decodedResponse = bd.decodeBuffer(response);
   Signature signChecker = Signature.getInstance("MD5withRSA");
   signChecker.initVerify(generatedPublicKey);
   signChecker.update(request.getBytes());
   
   if (signChecker.verify(decodedResponse))
   {
       System.out.println("verified");
      return true;
   }    
   else
   {
       System.out.println("verification failed");
    return false;
   }
  } catch (Exception e) {
   System.out.println("unable to verify signature");
   return false;
  }
 }

public static void main(String[] args) 
{
if(args.length<3)
{
System.out.println("Enter Client's Public Key,challenge message,response as the parameters");
}
else
{
checkSignature cs=new checkSignature();
   FileOutputStream fos; 
   DataOutputStream dos;

   try {
       File file= new File("MyFile.txt");
       fos = new FileOutputStream(file);
       dos=new DataOutputStream(fos);
       if(cs.check(args[0],args[1],args[2]))
       {
       fos.write("1".getBytes());
       }
       else
         fos.write("0".getBytes());
       } catch (IOException e) {
          e.printStackTrace();
      }
//cs.check("MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvC+m32GusrDvN5tQyAlr7SEcgcyNTfLSHG14bgX2CQzz6z5saoYGZWI3blurN4M+yTLlYEQGcI5bqqoFy40V7b2UF+UtU7hlXF+0041qDgN6iQGca19mizpBsdHycYgR4NaXcEt1P3JNOczX9HDTqdXSP1wECQ1TmlDRLCFoqSwIDAQAB","HJRNmVTOmHsIAkNpFuaLOTDfHiQsqrehvBDaFckWxfJWPpQAeISCdnWsfgWhYkSp","E29/+fYsStOjcv6zBt/apQeOg5qnEg/QLDSmNyypBhAE+dMmg+M7KIUeRFIH5VgE316cGzbq8rIElqGADmLoOGbBU5RHlj5NDSHJC4fbWEifsLhRrswfEIYxSJo5tpE2aNifqSahy3xmO/Wgt4GicwPLa+E00VeF91pIIANtKjg=");
}
}
}
