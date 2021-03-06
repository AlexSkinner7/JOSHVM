package org.bouncycastle.operator;

import org.bouncycastle.asn1.ASN1ObjectIdentifier;
import org.bouncycastle.asn1.nist.NISTObjectIdentifiers;
import org.bouncycastle.asn1.oiw.OIWObjectIdentifiers;
import org.bouncycastle.asn1.pkcs.PKCSObjectIdentifiers;
import org.bouncycastle.asn1.x509.AlgorithmIdentifier;
import org.bouncycastle.util.Integers;

import com.joshvm.java.util.Collections;
import com.joshvm.java.util.HashMap;
import com.joshvm.java.util.Map;

public class DefaultSecretKeySizeProvider
    implements SecretKeySizeProvider
{
    public static final SecretKeySizeProvider INSTANCE = new DefaultSecretKeySizeProvider();

    private static final Map KEY_SIZES;

    static
    {
        Map keySizes = new HashMap();

        keySizes.put(new ASN1ObjectIdentifier("1.2.840.113533.7.66.10"), Integers.valueOf(128));

        keySizes.put(PKCSObjectIdentifiers.des_EDE3_CBC, Integers.valueOf(192));
        keySizes.put(PKCSObjectIdentifiers.id_alg_CMS3DESwrap, Integers.valueOf(192));
        keySizes.put(PKCSObjectIdentifiers.des_EDE3_CBC, Integers.valueOf(192));

        keySizes.put(PKCSObjectIdentifiers.pbeWithSHA1AndDES_CBC, Integers.valueOf(64));
        keySizes.put(PKCSObjectIdentifiers.pbeWithMD5AndDES_CBC, Integers.valueOf(64));

        keySizes.put(NISTObjectIdentifiers.id_aes128_CBC, Integers.valueOf(128));
        keySizes.put(NISTObjectIdentifiers.id_aes192_CBC, Integers.valueOf(192));
        keySizes.put(NISTObjectIdentifiers.id_aes256_CBC, Integers.valueOf(256));
        keySizes.put(NISTObjectIdentifiers.id_aes128_GCM, Integers.valueOf(128));
        keySizes.put(NISTObjectIdentifiers.id_aes192_GCM, Integers.valueOf(192));
        keySizes.put(NISTObjectIdentifiers.id_aes256_GCM, Integers.valueOf(256));
        keySizes.put(NISTObjectIdentifiers.id_aes128_CCM, Integers.valueOf(128));
        keySizes.put(NISTObjectIdentifiers.id_aes192_CCM, Integers.valueOf(192));
        keySizes.put(NISTObjectIdentifiers.id_aes256_CCM, Integers.valueOf(256));
        keySizes.put(NISTObjectIdentifiers.id_aes128_wrap, Integers.valueOf(128));
        keySizes.put(NISTObjectIdentifiers.id_aes192_wrap, Integers.valueOf(192));
        keySizes.put(NISTObjectIdentifiers.id_aes256_wrap, Integers.valueOf(256));

        keySizes.put(OIWObjectIdentifiers.desCBC, Integers.valueOf(64));

        
        KEY_SIZES = Collections.unmodifiableMap(keySizes);
    }

    public int getKeySize(AlgorithmIdentifier algorithmIdentifier)
    {
        int keySize = getKeySize(algorithmIdentifier.getAlgorithm());

        // just need the OID
        if (keySize > 0)
        {
            return keySize;
        }

        // TODO: support OID/Parameter key sizes (e.g. RC2).

        return -1;
    }

    public int getKeySize(ASN1ObjectIdentifier algorithm)
    {
        Integer keySize = (Integer)KEY_SIZES.get(algorithm);

        if (keySize != null)
        {
            return keySize.intValue();
        }

        return -1;
    }
}
